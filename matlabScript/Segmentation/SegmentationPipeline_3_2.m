%% 3.2 LEAKAGE REMOVAL
%% INPUT:
%% FROM 2.3:MainAirwaySeg
%% FROM 3.1: AirwaySeg,AirwayEner
%% OUTPUT VARIABLES:
%%  G,GSub,skel: Codification of distal airways
%%  AirwaySeg: distal airways without leakage
%%  Leakage: distal airways leakage 
%%
%% POSSIBLE ACCELERATION: SEE 2.3
disp('LEAKAGE REMOVAL - start');
[ G,GSub,skel] = SegComplexity_23_10_2018( AirwaySeg );
Conn=26; %Segmentation Connectivity
[AirwaySeg,Leakage] =LeakageRemoval(AirwaySeg,MainAirwaySeg,GSub,AirwayEner,Conn);
save([ OutPutDataFolder filesep 'AirwaySegLeakage'],'AirwaySeg','G','GSub','skel','Leakage');
disp('LEAKAGE REMOVAL - done');