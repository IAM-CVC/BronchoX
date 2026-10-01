function [ G,GSub,Distal ] = SegComplexity_06_11_2017( BronchiSeg )

TargetNodes='Leaf';


% Lobular Division
[TBMask]=TracheaBronchiSep_Agnes(BronchiSeg);
TBDist=bwdist(TBMask);
Distal=bwlabeln((TBDist>0).*BronchiSeg,26);


G.endNodes=0;
G.totalPaths=0;


% Lobular Complexity
NBranch=max(Distal(:));
for kBranch=1:NBranch
    
    DistalBranch=double(Distal==kBranch);
    
        [ GSub{kBranch} ] = SegBranchLocalComplexity( DistalBranch,TBDist );
       
        G.endNodes=G.endNodes+ GSub{kBranch}.endNodes;
        G.totalPaths=G.totalPaths+GSub{kBranch}.totalPaths;

end

% Total Complexity
G.complexity=1-(G.endNodes)/G.totalPaths;

end

