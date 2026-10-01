function [ AirwaySeg ] = AirwaysSeg_from_Eners(AirwaySeg0,AirwayEner,AirwayWallEner,SegParam)

LungMaskROI=SegParam.LungMaskROI;
% Refinement of AirwaySeg0 (largest connected component connected to
% AirwaySeg0)
AirwaySeg=max(AirwayEner>SegParam.Th,AirwaySeg0);
AirwaySeg=bwlabeln(AirwaySeg,SegParam.Conn);
AirwaySeg=ismember(AirwaySeg,unique(AirwaySeg(find(AirwaySeg0(:)))));

% Remove Responses due to Lung borders
SegDistL2=bwdist(AirwaySeg);
WallMask=(AirwayWallEner>SegParam.ThWall).*(SegDistL2<=SegParam.ThBronchiDist);
AirwaySeg=PrincipalConnComp(double((1-WallMask).*AirwaySeg.*LungMaskROI),26,1); 


end

