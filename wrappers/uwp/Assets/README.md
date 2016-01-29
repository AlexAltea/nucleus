UWP Assets
==========

This folder contains assets, icons specifically, on which Visual Studio imposes some special naming requirements in order to successfully build the Universal Windows Platform project of Nucleus. Do **not** edit these files directly. Instead, modify the `/resources/icons/nucleus-alpha-*.png` files and copy them back to this folder renaming them to replace the file with the same resolution.

The reason we cannot fetch such files directly from `/resources/icons/` is:

```
error APPX0501: Validation error. error C00CE169: App manifest validation error: The app manifest must be valid as per schema: Line 15, Column 6, Reason: '..\..\resources\icons\nucleus-alpha-50x50.png' violates pattern constraint of '([^/\\]*[^./\\]+)(\\[^/\\]*[^./\\]+)*|([^/\\]*[^./\\]+)(/[^/\\]*[^./\\]+)*'.
```
