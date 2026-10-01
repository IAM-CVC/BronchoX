function [CITh,ThAct,UpDateParam]=Init_Th(BranchComplexity,DistalBranch,Distal,BronchiEner)

CITh=[0,0];
UpDateParam.AreaMxIncr=0;
UpDateParam.AreaMnIncr=0;

if(BranchComplexity==0)
    CITh(1)=min(BronchiEner(find(Distal(:))));
    CITh(2)=mean(BronchiEner(find(DistalBranch(:))));
    UpDateParam.AreaMxIncr=.5;
    UpDateParam.AreaMnIncr=0;
elseif(BranchComplexity>0)
    CITh(1)=min(BronchiEner(find(DistalBranch(:))));
    CITh(2)=max(BronchiEner(find(DistalBranch(:))));
    UpDateParam.AreaMxIncr=1;
    UpDateParam.AreaMnIncr=0.1;
end

ThAct=mean(CITh);