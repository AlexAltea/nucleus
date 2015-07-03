Keys Documentation
==================

Certain types of binary files used by the PlayStation 3 system are encrypted. For legal reasons, Nucleus cannot provide the decryption keys, hence the need for the users to dump the keys from their own legally purchased console. These keys have to be saved in a Keyvault file named `keys.xml` next to the Nucleus executable.

The Keyvault file has to be met following structure:

```xml
<?xml version="1.0" encoding="utf-8"?>
<keyvault>
  <self>
    [...]
  </self>
</keyvalut>
```


## SELF/SPRX Keys

In *Cell OS LV-2*, executable files need to be signed in order to be loaded. This applies both to ELF executables and PRX dynamic libraries (which are basically ELF's). Their signed versions are called SELF and SPRX respectively. The Keyvault must provide the corresponding decryption keys to load these files in the `<self>` node. These keys have to met following structure:

```xml
<key type="[A-Z0-9]+" version="[0-9A-F]{16}" revision="[0-9A-F]{4}">
  <ctype>[0-9A-F]{2}</ctype>
  <erk>[0-9A-F]{64}</erk>
  <riv>[0-9A-F]{32}</riv>
  <pub>[0-9A-F]{80}</pub>
  <priv>[0-9A-F]{42}</priv>
</key>
```

All attribute/node values above are regex strings. Aside from the for the key type, all values are specified by uppercase hexadecimal strings. For the key type following notiation is used: `LV0` (1), `LV1` (2), `LV2` (3), `APP` (4), `ISO` (5), `LDR` (6), `UNK7` (7), `NPDRM` (8).


## Example Keyvault file

This is just an example Keyvault file with fake keys. Don't expect it to work with any encrypted file:

```xml
<?xml version="1.0" encoding="utf-8"?>
<keyvault>
  <self>
    <key type="APP" version="0003005500000000" revision="000A">
      <ctype>2D</ctype>
      <erk>0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF</erk>
      <riv>0A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D</riv>
      <pub>8C9D0E1F2A3B4C5D0123456789ABCDEFA1B2C3D4E5F6A7BE5F6A7B8C9D0E1F2A8C9D0E1F2A3B4C5D</pub>
      <priv>000000000000000000000000000000000000000000</priv>
    </key>
    <key type="NPDRM" version="0003004200000000" revision="0004">
      <ctype>08</ctype>
      <erk>C5D0123456789ABCDEFA1B2C3D4E5F6A7BE5F6A7B8C9D0E1F2A8C9D0E1F2A3B4</erk>
      <riv>0123456789ABCDEF0123456789ABCDEF</riv>
      <pub>0A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D0A1B2C3D4E5F6A7B8C9D0E1F2A3B4C5D0A1B2C3D4E5F6A7B</pub>
      <priv>9ABCDEF0123456780A1B2C3D4E5F6A7B8C9D0E1F2A</priv>
    </key>
  </self>
</keyvalut>
```
