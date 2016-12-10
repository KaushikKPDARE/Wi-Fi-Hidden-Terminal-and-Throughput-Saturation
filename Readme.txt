Hidden Terminal Study
  - 3 Files (802.11a, 802.11b, 802.11g, 802.11n (2.4 GHz), 802.11n (5 GHz))
  - Considering Disable/Enable RTS/CTS.

Throughput Study
Case 1
  - 5 Files (802.11a, 802.11b, 802.11g, 802.11n (2.4 GHz), 802.11n (5 GHz))
  - Starts with 1 AP and 12 STA Nodes created.  AP node in the middle and the 12 STA nodes are each 50m away from AP.
  - 12 Active Flows.
	- For the study, this code was modified over and over reducing the number of active flows.

Case 2
  - 5 Files (802.11a, 802.11b, 802.11g, 802.11 (2.4 GHz), 802.11 (5 GHz))
  - Starts with 1 AP and 12 STA Nodes created.  AP node in the middle and the 12 STA nodes are each 50m away from AP.
  - 6 Active Flows.
	- For the study, this code was modifed over and over reducing the CBR rate.

Case 3
  - 5 Files (802.11a, 802.11b, 802.11g, 802.11 (2.4 GHz), 802.11 (5 GHz))
  - Starts with 1 AP and 2 STA Nodes created.  AP node in the middle and the 2 STA nodes are each 50m away from AP.
  - 2 Active Flows.
	- For the study, this code was modified over and over ireasing the number of nodes.  Note increasing # of nodes, but not increasing the # of active flows.


Implemenation Challenges:
  - How to place the nodes: After creating the STA Nodes, we had to figure how to place the nodes so that they are 50m away from the AP.  This was the major challenge when putting together the code.
  - Increasing the Active Flows: Had to make sure that it is only Node -> AP traffic.
