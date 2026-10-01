function [GSub]=Paths2BranchEntryPt_13_12_2017(GSub,DistalGlobalTh,MainLocalTh)

TargetNodes='Leaf';

[TBMask]=TracheaBronchiSep_Agnes(DistalGlobalTh);
MainDist1=bwdist(TBMask);
MainDist2=bwdist(1-DistalGlobalTh);
Distal=bwlabeln((1-TBMask).*DistalGlobalTh,26);

%%% Aixi es com es calcula GSub
%Distal=bwlabeln((TBDist>0).*skel,26);

for kS=1:length(GSub)
    DistalBranch=Distal==kS;
    [rootNode]= BranchRoot_1_12_2017(GSub{kS},MainDist1,MainDist2,DistalBranch);
    GSub{kS} = ComplexityPropsDeb(GSub{kS},TargetNodes,rootNode);
    GSub{kS}.rootNode=rootNode;
    
end