# LeonCode USTP maintenance

This repository is a fork of OpenWrt USTP. Original authorship and the complete
patch history are retained. Package builds are coordinated by
[leoncode-openwrt/packages](https://github.com/leoncode-openwrt/packages).

## Deliberately separate source lines

- `leoncode/router`: reviewed base `64752fd2` with the assignment macro fix
  backported separately. It does not include the AP-specific DSA/shutdown change.
- `leoncode/ap`: reviewed `8d50cbac`, including forwarding preservation during
  shutdown, bounded worker teardown, DSA BPDU reception and the assignment fix.

The common history includes bounded/coalesced worker queues, allocation/socket
cleanup, bridge priority propagation and regression coverage. Commit messages
identify each change. Compare the two branches before moving a fix between roles.

Source tests and SDK compilation do not establish hardware boot compatibility.
The October 2026 firmware boot failures are unresolved; package publication is
not firmware approval and never triggers device installation.
