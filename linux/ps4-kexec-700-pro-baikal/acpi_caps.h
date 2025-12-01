#ifndef ACPI_CAPS_H
#define ACPI_CAPS_H

#define UCHAR u8
#define USHORT u16
#define ULONG u32
#define PACKED __attribute__((packed))
typedef struct PACKED _PCI_CAPABILITIES_HEADER {
    UCHAR CapabilityID;
  	UCHAR Next;
} PCI_CAPABILITIES_HEADER, *PPCI_CAPABILITIES_HEADER;

typedef struct PACKED _PCI_MSI_CAPABILITY {
  PCI_CAPABILITIES_HEADER Header;
  u16 msiEnable : 1, multipleMessageCapable : 3, multipleMessageEnable : 3, address64Capable : 1, reserved0 : 8;
  u32 lowerAddress : 30, reserved1 : 2;
  union {
  	struct {
  		u32 upperAddress;
  		u32 messageData64 : 16, reservedData64 : 16;
  		u32 mask64;
  	};
  	struct {
  		u32 messageData32 : 16, reservedData32 : 16;
  		u32 mask32;
  	};
  };
} PCI_MSI_CAPABILITY, *PPCI_MSI_CAPABILITY;

#endif
