function G = ComplexityPropsDeb(G,TargetNodes,varargin)

%   G -> Graph
%       G.v -> Node positions
%       G.e -> Adjacency matrix
%       G.wv -> Node labels or attributes. A value 0 indicates it is an
%       end point of the skeleton
%       G.we -> Edges labels or attributes --> access to second point of an edge between 1 and 5 nodes --> G.we{1,5}(2)
%       G.lp -> leaf points : points to the leaf if it is a endnode --> access to a first leaf point of the first leaf of the node 2 --> G.we{2,1}(1)

%this function adds a new prop:  #Paths From root to endnodes
%       G.complexity =  1-#endNodes/#pathsToEndNode --> comlexity=0
%       indicates a perfect tree; complexity --> 1 indicates a tree with
%       loops
%       G.totalPaths = #pathsToEndNodes
%       G.endNodes  = #endNodes
%       G.pathsToEndNode -> nodes for all paths from root to endNode --> access to a first node of the first path to the endNode 200 --> G.pathsToEndNode{200,1}(1)
%       G.distality -> valid endnodes: (#endNodes with 1 path)/#endnodes.
%       Perfect tree has distality=1
paths=0;

if(isempty(G.e))
    G.complexity = 0;
G.pathsToEndNode = {};
G.endNodes = 0;
G.totalPaths = 0;
G.npahtsToNodes=[];
G.distality=1;
return;
end
%convert the graph to directed one
rootNode=1;
if(~isempty(varargin))
    rootNode=varargin{1};
end
adjNew = Graph2dirGraph(G.e,rootNode);

switch TargetNodes
    case 'SkelEE'
        %index of Skel endNodes
        endNodes = find(G.wv==0);
    case 'SkelEELeaf'
        endNodes = find(G.wv==0);
        %index of Tree endNodes (leaves)
        Leafs=find(sum(adjNew>0,2)==0);
        %Leafs=find(sum(G.e>0)==1);
        endNodes=intersect(endNodes,Leafs);
    case 'Leaf'
        endNodes=find(sum(adjNew>0,2)==0);
        % endNodes=find(sum(G.e>0)==1);
end

%calculate for each endNode all possible paths
%donem el node destí com el últim d'un DFS
gr = digraph(adjNew);
if(~isempty(varargin))
    DFS = dfsearch(gr, varargin{1});
else
    DFS = dfsearch(gr,1);
end
% Keep only nodes that can be reached from root.
endNodes=intersect(endNodes,DFS);

pahtsToNodes{size(G.e,1),:} = []; %NEW!

G.complexity = 0;
G.pathsToEndNode = {};
G.endNodes = numel(endNodes);
G.totalPaths = 0;
G.npahtsToNodes=[];
G.distality=1;

if(~isempty(endNodes))
    %% MODIFICAT 08/06/2017. Fent DFS(end) complexity pot donar negatiu pq no troba tots els paths
    %% diria que el que cal es que se li passi un endNode
    [pth,npahtsToNodes,pahtsToNodes] = pathbetweennodes_carles(adjNew, DFS(1), DFS(end),endNodes, false);
    %[pth,npahtsToNodes,pahtsToNodes] = pathbetweennodes_carles(adjNew,DFS(1), endNodes(1),endNodes, false);
    
    %% OBS: pahtsToNodes No conté tots els camins a una fulla, per obtenir-los jo he fet la cutrada (ComplexityProps_Deb) de:
    %     for k=1:length(endNodes)
    %     [pahtsToNodes{endNodes(k)}] = pathbetweennodes(adjNew, DFS(1), endNodes(k));
    %     npahtsToNodes(endNodes(k))=length(pahtsToNodes{endNodes(k)});
    %     end
    
    G.pathsToEndNode = pahtsToNodes;
    G.endNodes = numel(endNodes);
    G.totalPaths = sum(npahtsToNodes);
    G.npahtsToNodes=npahtsToNodes;
    
    %% MODIFICACIÓ 25/10/2017 PER GESTIONAR GRAFS AMB UN SOL NODE
    %     if(npahtsToNodes<Inf)
    %         G.complexity = 1-numel(endNodes)/sum(npahtsToNodes);
    %         G.totalPaths = sum(npahtsToNodes);
    %         G.distality=sum(G.npahtsToNodes(endNodes)==1)/length(endNodes);
    %     else
    %         G.complexity = 1;
    %         G.totalPaths = Inf;
    %         G.distality=0;
    %     end
    
    if(length(npahtsToNodes)>1)
        G.complexity = 1-numel(endNodes)/sum(npahtsToNodes);
        G.totalPaths = sum(npahtsToNodes);
    else
        G.endNodes =0;
    end
    
    
end
end