# LeonCode USTP

OpenWrt STP/RSTP daemon with reviewed LeonCode fixes and regression tests.
See [LEONCODE.md](LEONCODE.md) for the shared router and access-point source,
patch history and verification limits.

Packages and automated release builds: [leoncode-openwrt/packages](https://github.com/leoncode-openwrt/packages).

## TODO: upstream-ready shutdown and ownership handover

- [ ] Define how bridge control is handed back to the network manager on stop,
  restart and bridge removal. Distinguish intentional STP disablement from a
  temporary daemon outage.
- [ ] Ensure that a previously blocked port can become usable after a topology
  change. Freezing the last port states indefinitely is not a general solution.
- [ ] Avoid introducing forwarding loops when redundant links exist. Opening
  every port unconditionally is not a general loop-safe fallback either.
- [ ] Evaluate an explicit, coordinated fallback controller where appropriate,
  including takeover failure and recovery; do not assume automatic kernel STP.
- [ ] Add integration tests for a single uplink and redundant links, link loss,
  daemon stop/restart, takeover and restoration of normal RSTP operation.

Current behavior restores managed ports to forwarding on orderly teardown.
The unified source retains that behavior; this TODO is not an implemented fix
or a claim of safe operation without a running spanning-tree controller.
