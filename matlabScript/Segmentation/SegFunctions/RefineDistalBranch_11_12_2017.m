function [DistalBranchNew,BranchComplexity,GSub]=RefineDistalBranch_11_12_2017(InputEners,InputMasks,SegParam)


%% Input Parameters
% Energy Maps
AirwaysEnerBranch=InputEners.AirwaysEner;
AirwaysWallEnerBranch=InputEners.AirwaysWallEner;

%% Compute Local Segmentation
DistalSegIni=InputMasks.MainBronchi;
Distal=bwconncomp((InputMasks.DistalReg{1}).*DistalSegIni,26);
NReg=length(Distal.PixelIdxList);
DistalBranchNew=DistalSegIni;
DistalBranchNewAll=DistalBranchNew;
SegParamBranch=SegParam;
BranchComplexity=zeros(1,NReg);
GSub={};
for k=1:NReg
    %Current Branch
    InputMasks.MainBronchi=0*DistalSegIni;
    InputMasks.MainBronchi(Distal.PixelIdxList{k})=1;
    %Branch Extension
    SegParamBranch.ThAll=SegParam.Th(k);
    [ DistalBranchExt] = DistalLocalSeg_12_12_2017(InputMasks,AirwaysEnerBranch,AirwaysWallEnerBranch,SegParamBranch );
    %Branch Update provided Complexity is 0
    [ DistalBranchTmp,BranchComplexity(k),GSub{k}] = LocalSegUpdate_31_10_2017( InputMasks.MainBronchi,DistalBranchExt,SegParam );
    DistalBranchNew=max(DistalBranchNew,DistalBranchTmp);
    DistalBranchNewAll=max(DistalBranchNewAll,DistalBranchExt);
end


