function [GSub]=LoopNodes2GraphLoop(G,Demes)

GSub.e=G.e(Demes,Demes);
GSub.v=G.v(Demes,:);
GSub.wv=G.wv(Demes);
for kNode=1:length(Demes)
    GSub.we{kNode,:}={G.we{Demes(kNode),Demes}};
    GSub.lp{kNode,:}={G.lp{Demes(kNode),:}};
end
end