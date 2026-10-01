function [rootNode]=BranchRoot(GSub,MainDist)

skelEE=find(GSub.wv==0);

rootNode=1;

if(~isempty(skelEE))
    for kNode=1:length(skelEE)
        indEdge=find(1-cellfun('isempty',{GSub.we{skelEE(kNode),:}}));
        point=[ GSub.we{skelEE(kNode),indEdge}];
        indNode=sub2ind(size(MainDist),round(GSub.v(skelEE(kNode),1)),round(GSub.v(skelEE(kNode),2)),round(GSub.v(skelEE(kNode),3)));
        point=[point indNode];
        Dist2Main(kNode)=min(MainDist( point));
    end
    [~,rootNode]=min(Dist2Main);
    rootNode=skelEE(rootNode);
end
