# Changelog

## v2.0.0 (2026-11-23)

### Breaking changes

Your YAML needs to be updated - see [Upgrading from older versions](README.md#upgrading-from-older-versions). ESPHome shows a warning with a link to these notes when it finds the old configuration.

* `smartboiler:` is a single mapping instead of a list. A list with one item is still accepted (with a warning), only one water heater per ESP is supported.
* The *Monitoring* switch is created by the component, remove the `switch:` with `platform: ble_client`.
* All entities are created automatically with default names. Entity names in YAML are optional.
* The pairing UID is stored under a key derived from the water heater MAC address and no longer depends on entity names. The UID stored by older versions is found and migrated automatically on the first start.
* Requires ESPHome 2026.1 or newer.
* Raw packet dumps and other frequent messages are logged at `VERBOSE` level instead of `DEBUG`.

### New entities

* Status: low tariff, problem, error, model, capacity, PROG temperature.
* Controls: monitoring, HDO decoder and anti-legionella switches, PROG low/high temperature, holiday start/length, reset energy button.

### Fixes

* Unconfirmed requests are re-sent after 10 s and dropped after 30 s, rejected requests are handled.
* Authentication is retried until the water heater responds.
* Firmware version is requested on connect.
* STOP, TEST and HOLIDAY modes are reported correctly, thermostat shows OFF in STOP mode.
* No crash when optional entities are not configured, values from the water heater are validated.
* Water heater clock is not adjusted twice while an adjustment is pending.
* Builds with ESPHome 2026.11.
* Possible buffer overflow with a long firmware version string from the water heater.
* Protocol packets, received texts and request queues use fixed-size buffers instead of heap allocations (prevents heap fragmentation on long uptimes).

## v1.0.0

Last version with the original configuration format (list of water heaters, explicit entity names, `switch` with `platform: ble_client`).
