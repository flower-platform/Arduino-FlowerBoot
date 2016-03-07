# Arduino-FlowerBoot
Stage 2 bootloader to support OTA updates

# Integrating into FlowerOTA library as C array
Build project, open "Debug\FlowerBoot.bin" in "HxD" hex editor and export as C array: "File" -> "Export" -> "C"
Then, the exported C source can be integrated into the FlowerOTA library (FlowerOTA.h).
