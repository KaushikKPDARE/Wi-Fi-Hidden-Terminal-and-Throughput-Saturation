#include "ns3/core-module.h"
#include "ns3/propagation-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"

// This example considers three hidden stations in an 802.11n network.
// The user can specify whether RTS/CTS is used.
//
// Example: ./waf --run "simple-ht-hidden-stations"
//
// Network topology:
//
//   Wifi 192.168.1.0
//
//        AP
//   *    *    *    *
//   |    |    |    |
//   n1   n2   n3   n4

// 
// Packets in this simulation aren't marked with a QosTag so they are considered
// belonging to BestEffort Access Class (AC_BE).
// Authors: Kaushik Padmanabhan, Umesh Chanumolu and John Li

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Hidden/NotHiddenStations");

double stand_a(bool enableCtsRts, enum ns3::WifiPhyStandard standard)
{
  uint32_t payloadSize = 1472; //bytes
  uint64_t simulationTime = 10; //seconds

  //bool enableCtsRts=1;

//for(uint32_t i=0;i<=1;i++){
	//enableCtsRts=i;
  /*CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("enableCtsRts", "Enable RTS/CTS", enableCtsRts); // 1: RTS/CTS enabled; 0: RTS/CTS disabled
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.Parse (argc, argv);*/

  UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (3);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (50.0, 0.0, 0.0)); //position of first station node
  positionAlloc->Add (Vector (0.0, 0.0, 0.0)); //position of AP node
  positionAlloc->Add (Vector (100.0, 0.0, 0.0)); //position of second station node
  positionAlloc->Add (Vector (150.0, 0.0, 0.0)); //position of third station node
  mobility.SetPositionAllocator (positionAlloc);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);
  
  Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();
  lossModel->SetPathLossExponent (3);

  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());
  
   WifiHelper wifi;
    wifi.SetStandard (standard);	// standard is 802.11n
    if (standard == WIFI_PHY_STANDARD_80211n_2_4GHZ || standard == WIFI_PHY_STANDARD_80211n_5GHZ)	// using OFDM Rate 6_5Mbps BW 20MHz
    {
        wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue ("OfdmRate6_5MbpsBW20MHz"),
                                      "ControlMode",StringValue ("OfdmRate6_5MbpsBW20MHz"));
    }
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  phy.SetChannel (wifiChannel);

  HtWifiMacHelper mac = HtWifiMacHelper::Default ();
  //NqosWifiMacHelper mac;
  Ssid ssid = Ssid ("80211n");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "BeaconInterval", TimeValue (MicroSeconds (102400)),
               "BeaconGeneration", BooleanValue (true));

  NetDeviceContainer apDevice;
  apDevice = wifi.Install (phy, mac, wifiApNode);

  // Setting mobility model


  // Internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer StaInterface;
  StaInterface = address.Assign (staDevices);
  Ipv4InterfaceContainer ApInterface;
  ApInterface = address.Assign (apDevice);

  // Setting applications
  UdpServerHelper myServer (9);
  ApplicationContainer serverApp = myServer.Install (wifiApNode);
  serverApp.Start (Seconds (0.0));
  serverApp.Stop (Seconds (simulationTime + 1));

  UdpClientHelper myClient (ApInterface.GetAddress (0), 9);
  myClient.SetAttribute ("MaxPackets", UintegerValue (42949));
  myClient.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  myClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  // Saturated UDP traffic from stations to AP
  ApplicationContainer clientApp1 = myClient.Install (wifiStaNodes);
  clientApp1.Start (Seconds (1.0));
  clientApp1.Stop (Seconds (simulationTime + 1));
  
  Simulator::Stop (Seconds (simulationTime + 1));

  Simulator::Run ();
  Simulator::Destroy ();

  uint32_t totalPacketsThrough = DynamicCast<UdpServer> (serverApp.Get (0))->GetReceived ();
  double throughput = totalPacketsThrough * payloadSize * 8 / (simulationTime * 1000000.0);
  //std::cout << "Throughput: " << throughput << " Mbit/s" << '\n';
//}
  return throughput;
}

int main (int argc, char **argv)
{
 double output;
 for(int i=0;i<2;i++){
   if(i==0){
    std::cout << "\n"; 
	std::cout << "Hidden station experiment with RTS/CTS disabled:\n";
	std::cout << "WiFi Standard of 802.11n 5GHz \n" ;
    std::cout << "---------------------------------------\n";   
   
    std::cout << "Station nodes: 3 \n" << std::flush;
    output = stand_a (0,WIFI_PHY_STANDARD_80211n_5GHZ);
    std::cout << "Throughput: " << output << " Mbit/s" << '\n';
    std::cout << "---------------------------------------\n";
	std::cout << "\n";
	std::cout << "\n"; 
	
    std::cout << "WiFi Standard of 802.11n 2.4GHz \n" ;
    std::cout << "---------------------------------------\n";
     
    std::cout << "Station nodes: 3 \n" << std::flush;
    output = stand_a (0,WIFI_PHY_STANDARD_80211n_2_4GHZ);
    std::cout << "Throughput: " << output << " Mbit/s" << '\n';
    std::cout << "---------------------------------------\n";
    std::cout << "\n";
   }
   else if(i==1){
    std::cout << "\n"; 
	std::cout << "Hidden station experiment with RTS/CTS enabled:\n";
	std::cout << "WiFi Standard of 802.11n 5GHz \n" ;
    std::cout << "---------------------------------------\n";   
   
    std::cout << "Station nodes: 3 \n" << std::flush;
    output = stand_a (1,WIFI_PHY_STANDARD_80211n_5GHZ);
    std::cout << "Throughput: " << output << " Mbit/s" << '\n';
    std::cout << "---------------------------------------\n";
	std::cout << "\n";
	std::cout << "\n"; 
	
	std::cout << "WiFi Standard of 802.11n 2.4GHz \n" ;
    std::cout << "---------------------------------------\n";
     
    std::cout << "Station nodes: 3 \n" << std::flush;
    output = stand_a (1,WIFI_PHY_STANDARD_80211n_2_4GHZ);
    std::cout << "Throughput: " << output << " Mbit/s" << '\n';
    std::cout << "---------------------------------------\n";
	std::cout << "\n";
   }
  }
	
	return 0;	
}