#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../src/font-awesome/AwesomeGlobal.h"
#include "../../utils/Requests.h"
#include "../../utils/ProtocolUtils.h"
#include "../../encrypting/sodium/backends/SodiumCryptoBackend.h"

#include <curl/curl.h>

#include <QFont>
#include <QDebug>
#include <QString>
#include <QSettings>
#include <QMetaObject>
#include <QMessageBox>
#include <QFile>
#include <QToolTip>
#include <QTimer>
#include <QClipboard>
#include <QFontDatabase>
#include <QButtonGroup>

#include "../chat/delegates/ChatMessageDelegate.cpp"
#include "../contacts/delegates/ContactsDelegate.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings(new QSettings("config.ini", QSettings::IniFormat))
    , socketServer(nullptr)
    , socketServerThread(new QThread(this))
    , socketClient(nullptr)
    , clientSocketsThread(new QThread(this))
    , request(new Requests)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);

    QVariantMap iconOptions;
    iconOptions.insert("color", QColor("#e0e0e0"));
    iconOptions.insert("color-active", QColor("#e0e0e0"));

    ui->port_input->setText(QString::number(DEFAULT_SERVER_PORT));
    ui->write_to_button->setEnabled(false);

    ui->chat_stacked_widget->setCurrentIndex(1);

    ui->send_message_button->setIcon(awesome->icon(fa::fa_solid, fa::fa_paper_plane, iconOptions));
    ui->copy_server_button->setIcon(awesome->icon(fa::fa_solid, fa::fa_copy, iconOptions));
    ui->copy_server_button->setToolTip("Copy to clipboard");

    ui->chat_list->setItemDelegate(new ChatMessageDelegate(ui->chat_list));
    ui->chat_list->setWordWrap(true);
    ui->chat_list->setUniformItemSizes(false);
    ui->chat_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->chat_list->setSpacing(10);

    ui->contacts_list_view->setItemDelegate(new ContactsDelegate(ui->contacts_list_view));
    ui->contacts_list_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->contacts_list_view->setUniformItemSizes(true);
    // For hovering
    ui->contacts_list_view->setMouseTracking(true);
    ui->contacts_list_view->viewport()->setCursor(Qt::PointingHandCursor);

    // Set contacts model
    currentContactModel = new ContactListModel(this);
    ui->contacts_list_view->setModel(currentContactModel);
    ui->contacts_list_view->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->contacts_list_view->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->contacts_list_view, &QListView::clicked, this, &MainWindow::onContactClicked);

    QVector<Contact> contacts;
    currentContactModel->setContacts(contacts);

    // locale buttons
    QButtonGroup* localeGroup = new QButtonGroup(this);
    localeGroup->addButton(ui->en_locale, 0);
    localeGroup->addButton(ui->ru_locale, 1);
    localeGroup->addButton(ui->sindarin_locale, 2);

    connect(localeGroup, &QButtonGroup::idClicked, this, [=, this](int id){
        switch (id) {
            case 0: switchLanguage("en"); break;
            case 1: switchLanguage("ru"); break;
            case 2: switchLanguage("sindarin"); break;
        }
    });

    ui->en_locale->setIcon(QIcon(":/assets/images/en_flag.png"));
    ui->ru_locale->setIcon(QIcon(":/assets/images/ru_flag.png"));
    ui->sindarin_locale->setIcon(QIcon(":/assets/images/sindarin.png"));

    QSize iconSize(32, 32);
    ui->en_locale->setIconSize(iconSize);
    ui->ru_locale->setIconSize(iconSize);
    ui->sindarin_locale->setIconSize(iconSize);

    // Initializing cryptography
    baseCrypto = std::make_shared<SodiumCryptoBackend>();

    QIcon windowIcon(":/assets/images/logo.png");
    this->setWindowIcon(windowIcon);
    this->showMaximized();
    this->UploadConfig();
}

MainWindow::~MainWindow()
{
    QVariantList sizes;
    for (int& size : ui->splitter->sizes()) {
        sizes.append(size);
    }

    settings->setValue("splitterSizes", sizes);
    settings->sync();

    // To ensure that it will be destroyed in the same thread it is located, we gonna use deleteLater
    // No need to invoke here, server stops in the destructor (deleteLater calls it)
    if (socketServerThread && socketServerThread->isRunning()) {
        socketServerThread->quit();
        socketServerThread->wait();
    }

    if (clientSocketsThread && clientSocketsThread->isRunning()) {
        clientSocketsThread->quit();
        clientSocketsThread->wait();
    }

    if (socketServer){
        socketServer->deleteLater();
        socketServer = nullptr;
    }

    if (socketClient){
        socketClient->deleteLater();
        socketClient = nullptr;
    }

    delete socketServerThread;
    socketServerThread = nullptr;

    delete clientSocketsThread;
    clientSocketsThread = nullptr;

    delete settings;
    delete ui;
}


void MainWindow::initializeTranslatingTexts()
{
    ui->your_ip_label->setText(tr("Your address:"));
    ui->port_warning->setText(tr("Please, do not change port if you are not sure what are you doing."));
    ui->start_server_button->setText(tr("Start server"));
    ui->port_input->setPlaceholderText(tr("Your local port, use any from 30000 to 65535"));
    ui->write_to_button->setText(tr("Write to"));
    ui->client_address_input->setPlaceholderText(tr("Peer address"));
    ui->client_port_input->setPlaceholderText(tr("Peer port"));
    ui->status_label->setText(tr("Server status"));
    ui->welcome_text->setHtml(tr(R"(
        <b>Welcome!</b><br><br>

        To use this app, you must:<br>
        <ul>
          <li><b>Remember:</b> trust no one</li>
          <li><b>Understand:</b> the peer you're connecting to is recorded</li>
          <li><b>Know:</b> only your chat is secure</li>
        </ul>

        <p>
            To start chatting, you need <b>full IPv6 support</b> on your router.<br>
            If it's not available — contact your ISP.<br><br>

            Messages are <b>not stored</b>, nothing is stored — everything lives <b>only in your RAM</b>.
        </p>

        <p>
            Exchange copied addresses and the port where you started your server — using the <b>“Start Server”</b> button.<br>
            Once you receive your peer's address, insert their <b>IP and port</b> into the appropriate fields before clicking <b>“Write to”</b>.<br>
            Click the button. <b>Start chatting.</b>
        </p>
    )"));
    ui->ip_text->setText(this->stringSelfHostAddress);
}


// On show, after initing all of the UI, run internal configurations
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        PastInit();
    }
}


// Past init complex handler
void MainWindow::PastInit(){
    QString protocolName = "";
    if (!QCoreApplication::instance()->property("local_network").toBool()){
        QString externalIP = request->get("https://api64.ipify.org", true);
        QHostAddress addr;

        if (!addr.setAddress(externalIP)) {
            QMessageBox::warning(this, "Error", tr("Invalid IP address received: ") + externalIP);
            return;
        }
        if (addr.protocol() != QAbstractSocket::IPv6Protocol){
            QMessageBox::warning(this, "Error", tr("Your connection does not provide IPv6 address. Connection is unavailable."));
            // return;
        }
        if (addr.protocol() == QAbstractSocket::IPv4Protocol){
            protocolName = "/IPv4";
        } else if(addr.setAddress(externalIP) && addr.protocol() == QAbstractSocket::IPv6Protocol){
            protocolName = "/IPv6";
        }
        this->stringSelfHostAddress = externalIP + protocolName;
        this->selfHostAddress = addr;
    } else {
        QString address = getLocalIPv6Address();
        qDebug() << "Local IPv6 IP: " << address;
        protocolName = "/IPv6";
        this->selfHostAddress = QHostAddress(address);
        this->stringSelfHostAddress = address + protocolName;
    }
    // Initializing translator
    QSettings settings("config.ini", QSettings::IniFormat);
    QString langCode = settings.value("language", "en").toString();
    switchLanguage(langCode);
}

QString MainWindow::getLocalIPv6Address()
{
    const auto interfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface& iface : interfaces) {
        qDebug() << iface.type();
        if (!(iface.flags() & QNetworkInterface::IsUp) ||
            !(iface.flags() & QNetworkInterface::IsRunning) ||
            (iface.flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }

#ifdef Q_OS_WIN
    if (
        iface.type() != QNetworkInterface::Ethernet &&
        iface.type() != QNetworkInterface::Wifi
        )
        continue;
    QString name = iface.humanReadableName();

    if (name.contains("vEthernet", Qt::CaseInsensitive) ||
        name.contains("VMware", Qt::CaseInsensitive) ||
        name.contains("Virtual", Qt::CaseInsensitive) ||
        name.contains("TAP", Qt::CaseInsensitive))
        continue;
#elif defined(Q_OS_MAC)
    // en0 is base active interface, en1, en2... are additional for ethernet cabel/thunderbolt connection
    if (!iface.name().startsWith("en"))
        continue;
#endif

        int ifaceIndex = iface.index();

        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            QHostAddress ip = entry.ip();

            if (ip.protocol() != QAbstractSocket::IPv6Protocol || ip.isLoopback())
                continue;
            if (!ip.toString().startsWith("fe80"))
                continue;

            QString addr = ip.toString().section('%', 0, 0);

            addr += '%' + QString::number(ifaceIndex);

            return addr;
        }
    }

    return "";
}


// Initial configurations
void MainWindow::InitServer(int serverPort)
{
    // Move server to Thread
    socketServer = new IPv6ChatServer(this->selfHostAddress, serverPort);
    socketServer->moveToThread(socketServerThread);

    connect(socketServer, &IPv6ChatServer::messageArrived, this, &MainWindow::onMessageArrived);
    connect(socketServer, &IPv6ChatServer::clientDisconnected, this, &MainWindow::onServerClientDisconnected);
    connect(socketServer, &IPv6ChatServer::clientConnected, this, &MainWindow::onServerClientConnected);

    socketServer->cryptoBackend = baseCrypto->clone();

    socketServerThread->start();

    // Run server into the thread by invoking
    QMetaObject::invokeMethod(socketServer, [=, this]{
        socketServer->run();
    }, Qt::QueuedConnection);

    if (socketServerThread->isRunning()){
        ui->start_server_button->setDisabled(true);
        ui->port_input->setReadOnly(true);
    }
}

void MainWindow::InitClient()
{
    clientSocketsThread->start();

    QMetaObject::invokeMethod(clientSocketsThread, [this](){
        socketClient = new IPv6ChatClient();
        socketClient->cryptoBackend = this->baseCrypto->clone();
        connect(socketClient, &IPv6ChatClient::peerConnected, this, &MainWindow::onPeerConnected);
        connect(socketClient, &IPv6ChatClient::peerDisconnected, this, &MainWindow::onPeerDisconnected);
        connect(socketClient, &IPv6ChatClient::messageSent, this, &MainWindow::onMessageSent);
    }, Qt::QueuedConnection);
}

void MainWindow::UploadConfig()
{
    QVariantList  splitterSizes = settings->value("splitterSizes", QVariant::fromValue(ui->splitter->sizes())).toList();
    QList<int> sizes;
    for (QVariant& size : splitterSizes) {
        sizes.append(size.toInt());
    }
    ui->splitter->setSizes(sizes);
    if (sizes[0] < 150){
        QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
        QGridLayout *startServerGrid = qobject_cast<QGridLayout*>(ui->start_server_panel->layout());
        this->HideSidebarElements(ipGrid, startServerGrid);
    }
}

void MainWindow::switchLanguage(const QString &langCode)
{
    if (translator) {
        qApp->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }

    translator = new QTranslator(this);
    qDebug() << "Trying to load translation file:" << ":/translations/" + langCode + ".qm";
    if (translator->load(":/translations/" + langCode + ".qm")) {
        qDebug() << "Loaded translation";
        qApp->installTranslator(translator);
        ui->retranslateUi(this);

        if (langCode == "sindarin") {
            int fontID = QFontDatabase::addApplicationFont(":assets/fonts/tngan.ttf");
            if (fontID != -1) {
                QString family = QFontDatabase::applicationFontFamilies(fontID).at(0);
                QFont tengwarFont(family);
                qApp->setFont(tengwarFont);
            } else {
                qDebug() << "Unable load font";
            }
        } else {
            qApp->setFont(QFont("Segoe UI, Roboto"));
        }

        settings->setValue("language", langCode);
        settings->sync();
    }else{
        qDebug() << "Failed to load translation";
    }
    qDebug() << "Language chosen: " << langCode;
    this->applyStyleSheet(langCode);
    this->initializeTranslatingTexts();
}

void MainWindow::applyStyleSheet(QString langCode)
{
    QFile file(":/assets/styles/mainwindow.qss");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Failed to load stylesheet";
        return;
    }

    qDebug() << "Stylesheet loaded";

    QString style = QString::fromUtf8(file.readAll());
    file.close();

    QString fontName = (langCode == "sindarin") ? "\"Tengwar Annatar\"" : "\"Segoe UI\", \"Roboto\", sans-serif";
    style.replace("{{font}}", fontName);

    qApp->setStyleSheet(style);
}

// Chat pages changing handler
void MainWindow::openChatPage(const QString& chatID, const QString& clientID)
{
    // Clear: previous chat models from RAM
    if (chatID != currentChatID){
        if (currentMessageModel){
            ui->chat_list->setModel(nullptr);
            delete currentMessageModel;
            currentMessageModel = nullptr;
        }

        QVariantMap iconOptions;
        if (connectedClients.contains(chatID)) {
            iconOptions.insert("color-disabled", QColor("#03da5a"));
            isCurrentChatClientOnline = true;
            ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_check, iconOptions));
            ui->status_text->setText(tr("Online"));
        } else {
            iconOptions.insert("color-disabled", QColor("#d32f2f"));
            isCurrentChatClientOnline = false;
            ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_times, iconOptions));
            ui->status_text->setText(tr("Offline"));
        }

        currentChatID = chatID;
        ui->clientID_text->setText(clientID);

        // Set up messages in RAM for all chats (to store them as in DB)
        setUpMessagesForChatInRAM(chatID);

        currentContactModel->setActive(chatID);
        // 0 for chat page, initital is 1 (just in case)
        if(ui->chat_stacked_widget->currentIndex()){
            ui->chat_stacked_widget->setCurrentIndex(0);
        }
    }
}

void MainWindow::setUpMessagesForChatInRAM(const QString& chatID)
{
    if (!messages.contains(chatID)) {
        messages[chatID] = QList<Message>();
    }

    currentMessageModel = new MessageListModel(this);
    if (messages.contains(chatID))
        currentMessageModel->setMessages(messages[chatID]);

    ui->chat_list->setModel(currentMessageModel);
}

// Custom emited thread Signals

// Client
void MainWindow::onPeerConnected(const QString& clientID)
{
    QMessageBox::information(this, "INFO", tr("Connected to the peer: ") + clientID);

    // Save clientID to use later in DB/File/Cache.
    QString chatID = makeChatID(selfHostAddress.toString(), stripPort(clientID));
    openChatPage(chatID, clientID);
}

void MainWindow::onPeerDisconnected(const QString& clientID)
{
    QMessageBox::warning(this, "WARNING", tr("No connection to peer: ") + clientID);
}

void MainWindow::onMessageSent(const QString& clientID, const QByteArray& message)
{
    qDebug() << "Message sent: " << message << clientID;
    QString chatID = makeChatID(selfHostAddress.toString(), stripPort(clientID));
    // TODO: add optional ability to store into DB, for now only RAM
    messages[chatID].append({clientID, QString::fromUtf8(message), false});

    currentContactModel->onNewMessage(chatID, clientID, message);

    if (currentMessageModel)
        currentMessageModel->addMessage({clientID, QString::fromUtf8(message), false});

    ui->send_message_input->clear();

    ui->chat_list->scrollToBottom();
    currentContactModel->setActive(chatID);
}

//Server
void MainWindow::onMessageArrived(const QString& clientID, const QByteArray& message)
{
    qDebug() << "Message arrived: " << message << clientID;
    QString chatID = makeChatID(selfHostAddress.toString(), stripPort(clientID));
     // TODO: add optional ability to store into DB, for now only RAM
    messages[chatID].append({clientID, QString::fromUtf8(message), true});

    currentContactModel->onNewMessage(chatID, clientID, message);

    if (currentMessageModel)
        currentMessageModel->addMessage({clientID, QString::fromUtf8(message), true});

    if (chatID == currentChatID){
        ui->chat_list->scrollToBottom();
        currentContactModel->setActive(chatID);
    }
}

void MainWindow::onServerClientConnected(const QString& clientID)
{
    QString chatID = makeChatID(selfHostAddress.toString() , stripPort(clientID));
    if (chatID == currentChatID){
        QVariantMap iconOptions;
        iconOptions.insert("color-disabled", QColor("#03da5a"));
        isCurrentChatClientOnline = true;
        ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_check, iconOptions));
        ui->status_text->setText(tr("Online"));
    }
    connectedClients.insert(chatID);
}

void MainWindow::onServerClientDisconnected(const QString& clientID)
{
    QString chatID = makeChatID(selfHostAddress.toString(), stripPort(clientID));
    connectedClients.remove(clientID);
    if (chatID == currentChatID){
        QVariantMap iconOptions;
        iconOptions.insert("color-disabled", QColor("#d32f2f"));
        isCurrentChatClientOnline = false;
        ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_times, iconOptions));
        ui->status_text->setText(tr("Offline"));
        QString message = tr("Peer is disconnected, if peer will be active again, just push the button \"Write to\" using actual port.");
        messages[chatID].append({chatID, message, true});
        currentMessageModel->addMessage({tr("System"), message, true});
    }
}

// Classic UI slot signals
void MainWindow::on_start_server_button_clicked()
{
    bool isValid = true;
    int port = ui->port_input->text().toInt(&isValid);

    if(30000 > port || port > 65535){
        isValid = false;
    }

    if(!isValid){
        ui->port_input->setStyleSheet("border: 1px solid #dc3545");
        QMessageBox::warning(this, "Error", tr("Provide port in range of 30000-65535"));
        return;
    }

    InitServer(port);
    InitClient();
    ui->write_to_button->setEnabled(true);
}

void MainWindow::on_port_input_textChanged()
{
    ui->port_input->setStyleSheet("border: 1px solid white");
}

void MainWindow::on_write_to_button_clicked()
{
    bool isValid = true;
    int port = ui->client_port_input->text().toInt(&isValid);

    if(30000 > port || port > 65535){
        isValid = false;
    }

    if(!isValid){
        QMessageBox::warning(this, "Error", tr("Provide port in range of 30000-65535"));
        return;
    }

    QHostAddress clientAddress;
    QString clientIP = ui->client_address_input->text();

    if (clientIP.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", tr("Client IP cannot be empty"));
        return;
    }
    if (!clientAddress.setAddress(clientIP)) {
        QMessageBox::warning(this, "Error", tr("Invalid IP address received: ") + clientIP);
        return;
    }
    if (clientAddress.protocol() != QAbstractSocket::IPv6Protocol){
        QMessageBox::warning(this, "Error", tr("Address is not IPv6 address. Connection is unavailable."));
        return;
    }

    QMetaObject::invokeMethod(clientSocketsThread, [this, clientAddress, port](){
        socketClient->connectToPeer(clientAddress.toString(), port);
    }, Qt::QueuedConnection);
}

void MainWindow::on_port_input_returnPressed()
{
    on_start_server_button_clicked();
}

void MainWindow::on_client_address_input_returnPressed()
{
    on_write_to_button_clicked();
}

void MainWindow::on_client_port_input_returnPressed()
{
    on_write_to_button_clicked();
}

void MainWindow::on_send_message_button_clicked()
{
    QString message = ui->send_message_input->toPlainText();
    if (message.trimmed().isEmpty()){
        ui->send_message_input->setFocus();
        return;
    }

    QString clientID = ui->clientID_text->text();
    QString selfHost = selfHostAddress.toString() + ":" + ui->port_input->text();
    QMetaObject::invokeMethod(clientSocketsThread, [this, selfHost, clientID, message](){
        socketClient->sendMessage(selfHost, clientID, message.toUtf8());
    }, Qt::QueuedConnection);
}

// UI handlers
void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    int sidebarWidth = ui->sidebar->width();
    QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
    QGridLayout *startServerGrid = qobject_cast<QGridLayout*>(ui->start_server_panel->layout());
    if (sidebarWidth < 150){
        this->HideSidebarElements(ipGrid, startServerGrid);
    }else{
        this->ShowSidebarElements(ipGrid, startServerGrid);
    }
}

void MainWindow::HideSidebarElements(QGridLayout *ipGrid, QGridLayout *startServerGrid)
{
    ui->your_ip_label->setVisible(false);
    ui->port_input->setVisible(false);
    ui->port_warning->setVisible(false);
    ui->client_port_input->setVisible(false);
    ui->client_address_input->setVisible(false);
    ui->write_to_button->setVisible(false);
    ipGrid->setColumnStretch(0, 1);
    ipGrid->update();
    startServerGrid->setColumnStretch(1, 0);
    startServerGrid->update();

}

void MainWindow::ShowSidebarElements(QGridLayout *ipGrid, QGridLayout *startServerGrid)
{
    ui->your_ip_label->setVisible(true);
    ui->port_input->setVisible(true);
    ui->port_warning->setVisible(true);
    ui->client_port_input->setVisible(true);
    ui->client_address_input->setVisible(true);
    ui->write_to_button->setVisible(true);
    ipGrid->setColumnStretch(1, 6);
    ipGrid->update();
    startServerGrid->setColumnStretch(1, 6);
    startServerGrid->update();
}

void MainWindow::on_copy_server_button_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(ui->ip_text->text().replace("/IPv6", "").replace("/IPv4", ""));

    showToolTipOnPosition(
        ui->copy_server_button,
        tr("Copied to clipboard")
    );
}

void MainWindow::onContactClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    QString clientID = index.data(ContactListModel::ClientIDRole).toString();
    QString chatID = index.data(ContactListModel::ChatIDRole).toString();
    openChatPage(chatID, clientID);
}

void MainWindow::showToolTipOnPosition(QWidget* widget, QString text)
{
    QPoint globalPos = widget->mapToGlobal(QPoint(widget->width() / 2, 0));
    QToolTip::showText(globalPos, text, widget);

    QTimer::singleShot(1500, []() {
        QToolTip::hideText();
    });
}
