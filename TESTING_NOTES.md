# Testing notes

## ✅ IPv6 Local Testing Notes

### This project supports local IPv6 communication and was tested across:

- Windows (interface index like %5, e.g. fe80::c3e:e516:261:d28e%5)

- macOS (interface index like %11, e.g. fe80::c3e:e516:261:d28e%11)

## 🔍 Key Observations

- Link-local addresses (those starting with fe80::) require a zone ID (%interfaceIndex) to function correctly.

- Each OS assigns different zone indices to interfaces — Windows %5 might correspond to macOS %11, depending on the active adapter.

- Make sure to:

- - Use QNetworkInterface::index() to determine correct %X for outgoing packets.

- - Preserve the zone ID when sending link-local addresses to avoid routing errors.

- - Avoid tunnel and virtual interfaces (vethernet, utun, etc.) unless explicitly intended.

## Usage
- From WIN to MAC you gonna use  `MAC_ADDRESS % WIN_ZONE_ID : MAC_PORT`here must refer to the local interface (Windows side) that’s connected to the same network.
- Same applies in reverse — on macOS, the %zone must correspond to the sending interface, not the destination's.
## ✅ Conclusion
Cross-platform IPv6 P2P messaging over local interfaces works as expected.
For global IPv6 support (without link-local scope), proper handshake and DNS/ND mechanisms will be required.
