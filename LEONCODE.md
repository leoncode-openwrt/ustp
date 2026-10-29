# LeonCode USTP maintenance

This repository is a fork of OpenWrt USTP. Original authorship and the complete
patch history are retained. Package builds are coordinated by
[leoncode-openwrt/packages](https://github.com/leoncode-openwrt/packages).

## One shared maintenance line

`main` is the source for routers and access points on every supported architecture.
It includes the `8d50cbac` fixes: forwarding restoration during shutdown, bounded
worker teardown, DSA BPDU reception and the assignment macro correction. There
is no role-specific source selection. Bridge priorities and port roles are runtime
configuration. Historical source lines are preserved with `archive/*` tags.

The common history includes bounded/coalesced worker queues, allocation/socket
cleanup, bridge priority propagation and regression coverage. Commit messages
identify each change. Package builds pin an exact ancestor commit of `main`.

## Shutdown behavior and upstream review

The existing shutdown patch releases managed ports into forwarding. It avoids
leaving a port blocked indefinitely after its controller has stopped, but it
does not provide loop protection while the daemon is absent. Redundant links
therefore require explicit coordination with the network manager. Freezing the
last states is not a general solution either: a formerly blocked link would not
be released when the topology changes. No new shutdown policy is introduced by
the source-line consolidation. A future upstream submission must separately
review ownership transfer, stop versus restart, and redundant-link behavior.

Source tests and SDK compilation do not establish hardware boot compatibility.
The October 2026 firmware boot failures are unresolved; package publication is
not firmware approval and never triggers device installation.
