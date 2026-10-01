function [ DistalBranch,BranchComplexity,GSub ] = LocalSegUpdate_31_10_2017( DistalBranch,DistalBranchExt,UpDateParam )

BranchComplexity=1;
GSub={};
AreaIncr=sum(abs(DistalBranchExt(:)-DistalBranch(:)))./sum(DistalBranch(:));
%CondA=(AreaIncr<=UpDateParam.AreaMxIncr).*(AreaIncr>=UpDateParam.AreaMnIncr);

CondA=(AreaIncr<=UpDateParam.AreaMxIncr);
if(CondA)
    
    [ GSub ] = SegBranchLocalComplexity( DistalBranchExt,'Leaf' );
    BranchComplexity=GSub.complexity;
    if(BranchComplexity==0)
        DistalBranch=DistalBranchExt;
    end
end

end

