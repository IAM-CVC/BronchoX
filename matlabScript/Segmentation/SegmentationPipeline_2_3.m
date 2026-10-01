%% 2.3 LEAKAGE REMOVAL
%% I CANNOT SKIP THIS STEP, SINCE SEGMENTATION OF MAIN AIRWAYS CAN ALREADY INCLUDE LEAKAGE
%% 
%% INPUT:MainAirwaySeg FROM 2.2
%% OUTPUT VARIABLES:
%% G: Graph encoding airways segmentation
%% GSub: Cell array of graphs encoding each segmental airway. 
%%       That is airways are split according to their lobular sector and its geometry and complesity is computed and stored in GSub
%% MainAirwaySeg: Segmentation of airways without leakage
%%
%% POSSIBLE ACCELERATION: 
%%  1. SegComplexity_06_11_2017 computes a score from the segmentation skeleton.
%%  I think 2 steps could be accelerated:
%%     1.1 The skeleton and score are computed for each airway branch given by the 
%%     connected components of the MainAirwaySeg after removing the trachea and main bronchi 
%%     (computed internally in TracheaBronchiSep_Agnes). 
%%     This is implemented in a loop over connected components. 
%%     1.2 Also the computation of skeleton is not efficient. 
%%     We are using a C++ code (SkeletonizationMatlab.mexw64)
%%  2. LeakageRemoval removes loops using the graph encoding (cell array GSub) 
%%  each branch. 
%%      2.1 GraphLeaks computes the loops of each GSub in a loop over GSub
%%      2.2 LeakageRemoval_Agnes removes graphloops from segmentation also in a serial implementation
%%  I did not implement these functions.
disp('Computer score - start');
[ G,GSub] = SegComplexity_06_11_2017(MainAirwaySeg);
save([ OutPutDataFolder filesep 'Grafs'],'G','GSub')
disp('Computer score - done');


disp('Leakage Removal - start');
Conn=26; %Segmentation Connectivity
[MainAirwaySeg] =LeakageRemoval(MainAirwaySeg,MainBronchiSeg,GSub,AirwayEner,Conn);
save([ OutPutDataFolder filesep 'MainAirwaySeg'],'MainAirwaySeg')
disp('Leakage Removal - done');