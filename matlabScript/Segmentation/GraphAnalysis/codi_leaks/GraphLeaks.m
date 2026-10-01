function [ nodes_elim_all ] = GraphLeaks( GSub )




%% LOOP NODES

%LoopNodes_all: guardem per cada branca els nodes que formen loops
LoopNodes_all=[];
for k=1:numel(GSub)
    TG=GSub{k};
    LoopNodes_all{k}=[];
    if(~isempty(TG.pathsToEndNode))
        TG.e = Graph2dirGraph(TG.e,TG.rootNode);
        [ GSubLeak ] = GraphLoops( TG );
        LoopNodes = [];
        for kL=1:length(GSubLeak)
            % Common Path: GSubLeak{kL}.LoopNodes contains nodes for GSubLeak{kL}.LeafNode
            LoopNodes=[LoopNodes GSubLeak{kL}.LoopNodes];
        end
        LoopNodes_all{k} = LoopNodes;
    end
end




%% REMOVE  BRANCH LOOPS

branques_on_hi_ha_loops = [];
for i=1:length(LoopNodes_all)
    if isempty(LoopNodes_all{i})==false
        branques_on_hi_ha_loops = [branques_on_hi_ha_loops, i];
    end
end

%desem els nodes que eliminarem i els grafs
nodes_elim_all =[];
TG_elim_all =[];
for k=1:numel(GSub)
    nodes_elim_all{k}=[];
    TG_elim_all{k} =[];
end

%per cada branca eliminem els loops
clear GSubLeak
GSubLeak={};
GSubCompact=GSub;
for k=1:length(branques_on_hi_ha_loops)
    i = branques_on_hi_ha_loops(k);
    TG = GSub{i};
    [nodes_elim, TG] = f_elimina_loops_branca(TG);
    nodes_elim_all{i}= nodes_elim;
    TG_elim_all{i} = TG;
 
    GSubCompact{i}=CompactGraph(TG);
   % GSubCompact{i}=ComplexityPropsDeb(TG,TargetNodes,GSubCompact{i}.rootNode);
    
    GSubLeak{k}=GraphLoop(GSub{i},nodes_elim);
    GSubLeak{k}.LoopNodes=nodes_elim;
    
end


end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [GSub]=GraphLoop(G,Demes)

GSub.e=G.e(Demes,Demes);
GSub.v=G.v(Demes,:);
GSub.wv=G.wv(Demes);
for kNode=1:length(Demes)
    GSub.we{kNode,:}={G.we{Demes(kNode),Demes}};
    GSub.lp{kNode,:}={G.lp{Demes(kNode),:}};
end
end
