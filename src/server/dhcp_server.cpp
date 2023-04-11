#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
    defined(__NT__) || defined(_WIN64)

#include "dhcp_server.h"

#include <Windows.h>
#include <dhcpsapi.h>
#include <iphlpapi.h>

#include <QtCore>
#include <iostream>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "dhcpsapi.lib")

WCHAR serverIpAddress[] = L"192.168.1.1";

void dhcpServerStart() {
  // Initialize the DHCP server
  DWORD dwError = 0;
  dwError = DhcpDsInit();
  if (dwError != ERROR_SUCCESS) {
    std::cout << "init DHCP server: " << dwError << std::endl;
  }

  LPDHCP_SERVER_INFO_ARRAY serverArray;
  dwError = DhcpEnumServers(0, NULL, &serverArray, NULL, NULL);
  if (dwError != ERROR_SUCCESS) {
    std::cout << "get DHCP server: " << dwError << std::endl;
  } else {
    auto count = serverArray->NumElements;
    for (size_t i = 0; i < count; i++) {
      auto server = serverArray[i];
      std::cout << server.Servers->ServerAddress << std::endl;
    }
  }

  DHCPDS_SERVER serverInfo = {0};
  serverInfo.ServerName = serverIpAddress;
  dwError = DhcpAddServer(0, NULL, &serverInfo, NULL, NULL);
  if (dwError != ERROR_SUCCESS) {
    //     printf("Error adding DHCP server: %d\n", dwError);
    //     // return 1;
    qDebug() << QString("Error adding DHCP server: %1").arg(dwError);
  }

  // create a subnet
  DHCP_IP_ADDRESS subnetAddress = inet_addr("192.168.1.0");
  DHCP_IP_ADDRESS subnetMask = inet_addr("255.255.255.0");
  WCHAR subnetName[] = L"LAN file share Subnet";
  //   WCHAR subnetComment[] = L"A subnet for LAN file share";

  DHCP_SUBNET_INFO subnetInfo = {0};
  subnetInfo.SubnetAddress = subnetAddress;
  subnetInfo.SubnetMask = subnetMask;
  subnetInfo.SubnetName = subnetName;

  auto dwResult = DhcpCreateSubnet(serverIpAddress, subnetAddress, &subnetInfo);

  if (dwResult != ERROR_SUCCESS) {
    qDebug() << QString("Create subnet error %1").arg(dwResult);
  }

  // create a range
  DHCP_IP_ADDRESS rangeStartAddress = inet_addr("192.168.1.100");
  DHCP_IP_ADDRESS rangeEndAddress = inet_addr("192.168.1.200");
  WCHAR rangeComment[] = L"A range for my subnet";

  DHCP_IP_RANGE range;
  range.StartAddress = rangeStartAddress;
  range.EndAddress = rangeEndAddress;

  DHCP_SUBNET_ELEMENT_DATA_V4 elementData;
  elementData.ElementType = DhcpIpRanges;
  elementData.Element.IpRange = &range;

  dwResult =
      DhcpAddSubnetElementV4(serverIpAddress, subnetAddress, &elementData);

  if (dwResult != ERROR_SUCCESS) {
    qDebug() << QString("Create subnet element error %1").arg(dwResult);
  }
}

void dhcpServerStop() {
  DHCPDS_SERVER serverInfo = {0};
  serverInfo.ServerName = serverIpAddress;
  DWORD dwError = DhcpDeleteServer(0, NULL, &serverInfo, NULL, NULL);
  if (dwError != ERROR_SUCCESS) {
    // printf("Error deleting DHCP server: %d\n", dwError);
    // return 1;
    qDebug() << QString("Error deleting DHCP server: %1").arg(dwError);
  }
}
#endif