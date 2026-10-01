%% 2.3 LEAKAGE REMOVAL
function [AirwaySeg,Leakage] =LeakageRemoval(AirwaySeg,MainAirwaySeg,GSub,AirwayEner,varargin)

Conn=26;
if(~isempty(varargin))
    Conn=varargin{1};
end

[GSub]=Paths2BranchEntryPt_13_12_2017(GSub,AirwaySeg,MainAirwaySeg);
[GSubb]=AddNodeEnergy2Graf(AirwayEner,MainAirwaySeg,GSub);

[ nodes_elim_all ]= GraphLeaks( GSubb );

[AirwaySeg,Leakage]=LeakageRemoval_Agnes(AirwaySeg,GSub,nodes_elim_all,Conn);




end
