function [ GSub ] = GraphLoops( G )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

GSub={};
% find Multiple Paths
indMultiple=find(G.npahtsToNodes>1);
for k=1:length(indMultiple)
    % Loop Paths
    Cami={G.pathsToEndNode{indMultiple(k),:}};
    Cami={Cami{find(1-cellfun(@isempty,Cami))}};
    Comu=Cami{1}(1:end-1);
    for kC=2:length(Cami)
        Comu=intersect(Comu,Cami{kC});
    end
    Comu=Cami{1}(ismember(Cami{1},Comu));
    
    Demes=setdiff([Cami{:}],Comu(1:end));
   
     % Loop Graph
  [GSub{k}]=GraphLoop(G,Demes);
  GSub{k}.LoopNodes=Demes;
  GSub{k}.Last=Comu(end); 
  GSub{k}.LeafNode=indMultiple(k);
  %Això haurien de ser els veins si el graf no esta espaialment ordenat (root is entry point to branch)
end


    
end

function [GSub]=GraphLoop(G,Demes)

GSub.e=G.e(Demes,Demes);
GSub.v=G.v(Demes,:);
GSub.wv=G.wv(Demes);
for kNode=1:length(Demes)
    GSub.we{kNode,:}={G.we{Demes(kNode),Demes}};
    GSub.lp{kNode,:}={G.lp{Demes(kNode),:}};
end
end