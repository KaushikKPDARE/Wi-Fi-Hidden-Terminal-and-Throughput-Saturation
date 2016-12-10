//-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- 
// wifi standanrd 802.11g without hidden terminal 
// Kaushik, Umesh and John
#include "ns3/core-module.h"
#include "ns3/propagation-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;
//run time = 10 secs
double randomStartTime(double seed);
double stand_a(uint32_t STAnode, enum ns3::WifiPhyStandard standard, std::string cbr)
{   
    uint32_t APnode = 1;
   
    // 1. Create n number of nodes and AP
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(STAnode);
    NodeContainer wifiAPNodes;
    wifiAPNodes.Create(APnode);
    
    // 2. Setup wifi channel, default constant, transmission range is set here?
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default(); // default PHY values
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    
	wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(100));	//
    wifiPhy.SetChannel(wifiChannel.Create());
   
    
    // 5. Install wireless devices
    WifiHelper wifi;
    wifi.SetStandard (standard);			// standard is 802.11g
    if (standard == WIFI_PHY_STANDARD_80211g)	
    {
        wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue ("DsssRate2Mbps"),
                                      "ControlMode",StringValue ("DsssRate2Mbps"));
    }
      
    Ssid ssid = Ssid("wifi-client");
    NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
    wifiMac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false)); // use stawifi
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid), "BeaconGeneration", BooleanValue(true));
    NetDeviceContainer apDevices;
    apDevices = wifi.Install(wifiPhy, wifiMac, wifiAPNodes);
    
    // Add mobility on AP node
    MobilityHelper mobility;
    // Set AP stationary, do not change on AP mobility stuff, AP is set to stationary
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");	// coordinate (0,0) on (x,y) axis for AP node
    mobility.Install(wifiAPNodes);
    
    mobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator","X", StringValue("0.0"), "Y", StringValue("0.0"), "rho", StringValue("50.0"));
  
    mobility.Install(wifiStaNodes);		// install STA node
    //// print locations
    Ptr<MobilityModel> apmob = wifiAPNodes.Get(0)->GetObject<MobilityModel>();
    Vector posap = apmob->GetPosition();
    std::cout<<"AP cordinates: x = "<<posap.x<<", y = "<<posap.y<<"\n";
    for (uint32_t k=0; k<STAnode; k++)
    {
        Ptr<MobilityModel> mob = wifiStaNodes.Get(k)->GetObject<MobilityModel>();
        Vector pos = mob->GetPosition();
        std::cout<<"Node "<<k<<" cordinates: x = "<<pos.x<<", y = "<<pos.y<<"\n";
    }
    // Install protocol stack
    InternetStackHelper stack;
    stack.Install(wifiStaNodes);
    stack.Install(wifiAPNodes);
       
    //  TCP stack &  IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.0.0.0", "255.0.0.0");
    ipv4.Assign (staDevices);
    ipv4.Assign(apDevices);
    
    Ptr<Node> apn = wifiAPNodes.Get(0);
    Ptr<Ipv4> ipv4add = apn->GetObject<Ipv4>();
    Ipv4Address APaddr = ipv4add->GetAddress(1,0).GetLocal();
    
    double starttime = 1.000000;
    double stoptime = 10.000000;
    double duration = stoptime - starttime;
    ApplicationContainer cbrApps;
    uint16_t cbrPort = 12345;
    StringValue cbrate = StringValue(cbr);		// cbr traffic rate set to change
    OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (APaddr, cbrPort));
    onOffHelper.SetAttribute ("PacketSize", UintegerValue (1400));
    onOffHelper.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    
    ApplicationContainer pingApps;
    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper(APaddr, echoPort);
    echoClientHelper.SetAttribute("MaxPackets", UintegerValue(1));
    echoClientHelper.SetAttribute("Interval", TimeValue(Seconds(0.1)));
    echoClientHelper.SetAttribute("PacketSize", UintegerValue(10));
    
    //change in start times
    for (uint16_t j=0; j<STAnode; j++)
    {
        double randTime = randomStartTime(starttime);
        double randEchoTime = randomStartTime(0.0);
        onOffHelper.SetAttribute ("DataRate", cbrate);
        onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (randTime)));
        cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get(j)));
        echoClientHelper.SetAttribute("StartTime", TimeValue(Seconds(randEchoTime)));
        pingApps.Add(echoClientHelper.Install(wifiStaNodes.Get(j)));
    }
    
    // 8. Install FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
    
    // 9. Run  for 10 seconds
    Simulator::Stop (Seconds (stoptime));
    Simulator::Run ();
    
    // 10. Print per flow statistics
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    double totalThroughput = 0.0;
    // int p = 0;
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      		if (i->first > STAnode)
            {
                Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
                std::cout << "Flow " << i->first - STAnode << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / duration / 1000 / 1000  << " Mbps\n";
                std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                totalThroughput = totalThroughput+i->second.rxBytes*8.0/duration/1000/1000;
                std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / duration / 1000 / 1000  << " Mbps\n";
            }
    }
    std::cout<<"Total throughput: "<<totalThroughput<<" Mbps\n";
   
    Simulator::Destroy ();
    return totalThroughput;
}

double randomStartTime(double seed)
{
    double min = seed;
    double max = min+0.01;
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable>();
    x->SetAttribute("Min", DoubleValue(min));
    x->SetAttribute("Max", DoubleValue(max));
    return x->GetValue();
}

int main (int argc, char **argv)
{
    
    double output;
    std::cout << "The following experiment results are done with NO HIDDEN TERMINAL \n" ;
    std::cout << " client nodes are increased for 802.11g standard to check saturation \n" ;
    std::cout << "cbr rate 2Mbps \n";
    std::cout << "---------------------------------------\n";
     
    std::cout << " station nodes 3 \n" << std::flush;
    output = stand_a (3, WIFI_PHY_STANDARD_80211g, "2Mbps");
    std::cout << "throughput: " << output << "\n";
    std::cout << "---------------------------------------\n";
     
    std::cout << " station nodes 6 \n" << std::flush;
    output = stand_a (6, WIFI_PHY_STANDARD_80211g, "2Mbps");
    std::cout << "throughput: " << output << "\n";
    std::cout << "---------------------------------------\n";
    
    std::cout << " station nodes 9 \n" << std::flush;
    output = stand_a (9, WIFI_PHY_STANDARD_80211g, "2Mbps");
    std::cout << "throughput: " << output << "\n";
    std::cout << "---------------------------------------\n";
      
    std::cout << " station nodes 12 \n" << std::flush;
    output = stand_a (12, WIFI_PHY_STANDARD_80211g, "2Mbps");
    std::cout << "throughput: " << output << "\n";
    std::cout << "---------------------------------------\n";
    return 0;
}