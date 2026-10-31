# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| Latest  | ✅        |

## Security Considerations

### Screenshot Capture
- LinuxShot respects Wayland security restrictions and uses XDG Desktop Portal for authorized capture
- No insecure workarounds to bypass compositor security
- PipeWire sessions are properly closed after capture

### File Operations
- Save operations respect file permissions
- No arbitrary file access outside user-selected directories
- Configuration stored in XDG-compliant locations

### Network
- Upload uses HTTPS only
- No telemetry or data collection
- No automatic connections without user action

### Build
- AddressSanitizer and UBSan available for debug builds
- No suppressed compiler warnings without documented justification

## Reporting a Vulnerability

Please report security issues via the project's issue tracker with the "security" label, or contact the maintainer directly.

