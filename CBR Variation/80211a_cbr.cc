//WiFi 802.11a (1 AP, 12 STA Nodes)
//Varying cbr
//Author: John, Kaushik, Umesh

  #include "ns3/core-module.h"
  #include "ns3/propagation-module.h"
  #include "ns3/network-module.h"
  #include "ns3/applications-module.h"
  #include "ns3/mobility-module.h"
  #include "ns3/internet-module.h"
  #include "ns3/flow-monitor-module.h"
  #include "ns3/wifi-module.h"
  
  using namespace ns3;
  
  void experiment (bool enableCtsRts)
  {
    // Enable or disable CTS/RTS
    UintegerValue ctsThr = (enableCtsRts ? UintegerValue (100) : UintegerValue (2200));
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);
  
    // Create 1 AP and 12 STA Nodes 
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create (12);
    NodeContainer wifiApNode;
    wifiApNode.Create (1);
    
    // Install wireless devices
    WifiHelper wifi;
    wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
    std::string phyMode ("OfdmRate6Mbps");
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", 
                                  "DataMode",StringValue (phyMode), 
                                  "ControlMode",StringValue (phyMode));
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
    YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
    wifiPhy.SetChannel (wifiChannel.Create ());
    NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
    
    Ssid ssid = Ssid ("ns-3-ssid");
    wifiMac.SetType ("ns3::StaWifiMac",
		     "Ssid", SsidValue (ssid),
		     "ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);
    wifiMac.SetType ("ns3::ApWifiMac",
		     "Ssid", SsidValue (ssid));
    NetDeviceContainer apDevices;
    apDevices = wifi.Install (wifiPhy, wifiMac, wifiApNode);
 
    // uncomment the following to have pcap output
    // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);
 
    // Add Mobility
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodes);
    mobility.Install (wifiApNode);
 
    Ptr<ConstantPositionMobilityModel> ApNode = wifiApNode.Get (0)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node1 = wifiStaNodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node2 = wifiStaNodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node3 = wifiStaNodes.Get (2)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node4 = wifiStaNodes.Get (3)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node5 = wifiStaNodes.Get (4)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node6 = wifiStaNodes.Get (5)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node7 = wifiStaNodes.Get (6)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node8 = wifiStaNodes.Get (7)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node9 = wifiStaNodes.Get (8)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node10 = wifiStaNodes.Get (9)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node11 = wifiStaNodes.Get (10)->GetObject<ConstantPositionMobilityModel> ();
    Ptr<ConstantPositionMobilityModel> Node12 = wifiStaNodes.Get (11)->GetObject<ConstantPositionMobilityModel> ();
    ApNode->SetPosition (Vector (0, 0, 0));
    Node1->SetPosition (Vector (0, 50, 0));
    Node2->SetPosition (Vector (30, 40, 0));
    Node3->SetPosition (Vector (40, 30, 0));
    Node4->SetPosition (Vector (50, 0, 0));
    Node5->SetPosition (Vector (40, -30, 0));
    Node6->SetPosition (Vector (30, -40, 0));
    Node7->SetPosition (Vector (0, -50, 0));
    Node8->SetPosition (Vector (-30, -40, 0));
    Node9->SetPosition (Vector (-40, -30, 0));
    Node10->SetPosition (Vector (-50, 0, 0));
    Node11->SetPosition (Vector (-40, 30, 0));
    Node12->SetPosition (Vector (-30, 40, 0));

    // Install TCP/IP stack & assign IP addresses
    InternetStackHelper internet;
    internet.Install (wifiApNode);
    internet.Install (wifiStaNodes);
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.0.0.0", "255.0.0.0");
    ipv4.Assign (apDevices);
    ipv4.Assign (staDevices);
		
    // Install applications: two CBR streams each saturating the channel 
    ApplicationContainer cbrApps;
    uint16_t cbrPort = 12345;
    OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.1"), cbrPort));
    onOffHelper.SetAttribute ("PacketSize", UintegerValue (1400));
    onOffHelper.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
 
    // flow 1:  node 1 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000000bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.000000)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (0))); 
  
    // flow 2:  node 3 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000100bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.001)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (2))); 

    // flow 3:  node 5 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000200bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.002)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (4))); 

    // flow 4:  node 7 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000300bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.003)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (6))); 
	
    // flow 5:  node 9 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000400bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.004)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (8))); 
	
    // flow 6:  node 11 -> AP
    onOffHelper.SetAttribute ("DataRate", StringValue ("6000500bps"));
    onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.005)));
    cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (10))); 

    // flow 7:  node 12 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3000600bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.006)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (11))); 

    // flow 8:  node 10 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3000700bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.007)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (9))); 

    // flow 9:  node 8 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3000800bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.008)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (7))); 

    // flow 10:  node 6 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3000900bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.009)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (5))); 

    // flow 11:  node 4 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3001000bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.010)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (3))); 

    // flow 12:  node 2 -> AP
    //onOffHelper.SetAttribute ("DataRate", StringValue ("3001100bps"));
    //onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.011)));
    //cbrApps.Add (onOffHelper.Install (wifiStaNodes.Get (1))); 
	
    uint16_t  echoPort = 9;
    UdpEchoClientHelper echoClientHelper (Ipv4Address ("10.0.0.1"), echoPort);
    echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
    echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
    ApplicationContainer pingApps;
  
    // again using different start times to workaround Bug 388 and Bug 912
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (0))); 
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.006)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (2)));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.011)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (4)));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.016)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (6)));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.021)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (8)));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.026)));
    pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (10)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.031)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (11)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.036)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (9)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.041)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (7)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.046)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (5)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.051)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (3)));
    //echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.056)));
    //pingApps.Add (echoClientHelper.Install (wifiStaNodes.Get (1)));
	
    // Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
   
    // Run simulation for 10 seconds
    Simulator::Stop (Seconds (10));
    Simulator::Run ();
   
    // Print per flow statistics
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        // first 2 FlowIds are for ECHO apps, we don't want to display them
        //
        // Duration for throughput measurement is 9.0 seconds, since 
        //   StartTime of the OnOffApplication is at about "second 1"
        // and 
        //   Simulator::Stops at "second 10".
        if (i->first > 6)
          {
            Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
            std::cout << "Flow " << i->first - 6 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
            std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
            std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
            std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
            std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
            std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
          }
    }
		
    // Cleanup
    Simulator::Destroy ();
  }
   
  int main (int argc, char **argv)
  {
    CommandLine cmd;
    cmd.Parse (argc, argv);
    
    std::cout << "Hidden station experiment with RTS/CTS disabled:\n" << std::flush;
    experiment (false);
    std::cout << "------------------------------------------------\n";
    std::cout << "Hidden station experiment with RTS/CTS enabled:\n";
    experiment (true);

    return 0;
  }
