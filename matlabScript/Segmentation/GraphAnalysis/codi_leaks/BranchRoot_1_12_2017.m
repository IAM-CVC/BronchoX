function [rootNode]=BranchRoot_1_12_2017(GSub,MainDist1,MainDist2,DistalBranch)

%%% LLenca
Tall=DistalBranch.*(MainDist1<2);
Conn=bwconncomp(Tall);
NComp=length(Conn.PixelIdxList);

for k=1:NComp
    RMax(k)=max(MainDist2(Conn.PixelIdxList{k}));
end
[~,ind]=max(RMax);
[TallPtsX,TallPtsY,TallPtsZ]=ind2sub(size(MainDist1),Conn.PixelIdxList{ind});


% Manage Empty trees
rootNode=1;
% Compute Closest Pt to Slide
Nodes=GSub.v;
NNodes=size(Nodes,1);
if(NNodes>0)
    for k=1:NNodes
        Dist2Main(k)=min(abs(TallPtsX-Nodes(k,1))+abs(TallPtsY-Nodes(k,2))+abs(TallPtsZ-Nodes(k,3)));
    end
    [~,rootNode]=min(Dist2Main);
end

end
