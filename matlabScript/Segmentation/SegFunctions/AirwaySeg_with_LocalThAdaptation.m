
%% 2.4 DISTAL REFINEMENT WITH LOCAL THRESHOLD ADAPTATION
function [AirwaySeg,SegParam]=AirwaySeg_with_LocalThAdaptation( AirwaySeg0,Leakage,AirwayEner,AirwayWallEner,SegParam)

% Distal ROI for Local Refinement
DistalRegParam.MidTh=4;
DistalRegParam.CloseSze=3;
DistalRegParam.ThA=10;
[DistalROI,G]=DistalRegion4LocRefinement_29_12_2017(AirwaySeg0,Leakage,DistalRegParam);


% Initial Segmentation Branches
Distal=bwlabeln((DistalROI).*AirwaySeg0,26);
NBranch=max(Distal(:));
DistalExt=0*Distal;

% Branch Refinement Common Input Data
CITh(1)=500;
CITh(2)=SegParam.Th;
ThAct=mean(CITh);
SegParam.Th=ThAct*ones(1,NBranch);
CITh=repmat(CITh,NBranch,1);

% Branch Refinement Parameters
InputEners.AirwaysEner=AirwayEner;
InputEners.AirwaysWallEner=AirwayWallEner;
InputMasks.MainBronchi=AirwaySeg0;
InputMasks.DistalReg{1}=DistalROI;


itCtr=1;
while((max(diff(CITh'))>SegParam.CITh_Diff))
    
    [AirwaySeg,BranchComplexity]=RefineDistalBranch_11_12_2017(InputEners,InputMasks,SegParam);
    %  InputMasks.MainBronchi=DistalLocalTh;
    %Update threshold
    [CITh,ThAct]=Update_Th_05_12_2017(BranchComplexity,CITh,SegParam.Th,0);
    SegParam.Th=ThAct;
    itCtr=itCtr+1;
end % end while branch refinement


