function [ G,GSub,skel] = SegComplexity_23_10_2018( BronchiSeg )

TargetNodes='Leaf';

skel=0*BronchiSeg;
skel = SkeletonizationMatlab(double(BronchiSeg));
%skel=Skeleton3D(BronchiSeg);
% Lobular Division
[TBMask]=TracheaBronchiSep_Agnes(BronchiSeg);
TBDist=bwdist(TBMask);
DistalSkel=bwlabeln((TBDist>0).*skel,26);


G.endNodes=0;
G.totalPaths=0;
GSub ={};

% Lobular Complexity
%% OBS: Això podria ser en parallel amb un parfor
NBranch=max(DistalSkel(:));
for kBranch=1:NBranch
    
         DistalSkBranch=double(DistalSkel==kBranch);

        [ GSub{kBranch}] = SegBranchLocalComplexity( DistalSkBranch,TBDist,DistalSkBranch );
       
        G.endNodes=G.endNodes+ GSub{kBranch}.endNodes;
        G.totalPaths=G.totalPaths+GSub{kBranch}.totalPaths;
        if(isinf(GSub{kBranch}.complexity))
            G.complexity=Inf;
            return;
        end

end

% Total Complexity
G.complexity=1-(G.endNodes)/G.totalPaths;

end

