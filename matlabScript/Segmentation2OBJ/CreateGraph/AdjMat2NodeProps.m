% Node es una llista d'estructures del nodes que estan conectats a kRoot.
% Cada element de la llista conté: Parent, Children, IsEnd, Level
% La Llista té forats (=Nodes buits) si la numeració dels nodes conectats
% al root no es consecutiva, ex 1,2,3,5
% NodeRoot es una llista d'estructures eliminant els nodes buits i, per
% tant, renumerant els nodes conectats al root.

function [Node,NodeRoot]=AdjMat2NodeProps(AdjMat,varargin)


%% Initialize Tree Iterator
% Children List to iterate graph
NBranch=size(AdjMat,1);
for k=1:NBranch
    NodeChild=AdjMat(k,:);
    NodeChild(k)=0;
    ChildList{k}=find(NodeChild);
    
end

% Lists of Visited and ToVisit Nodes to control Iteration Loop
kRoot=1;
if(~isempty(varargin))
    kRoot=varargin{1};
end
Node(kRoot).Level=0;
ToVisit=zeros(1,NBranch);
ToVisit(kRoot)=1;
Visited=zeros(1,NBranch);

%% Tree Iteration
it=1;
while(it<=NBranch)
    Node2Visit=setdiff(find(ToVisit),find(Visited));
    for CP=1:length(Node2Visit)
        CurrentParent=Node2Visit(CP);
        Children_All=ChildList{CurrentParent};
        for kCh=1:length(Children_All)
            CurrentNode=Children_All(kCh);
            % Create Node
            Node(CurrentNode).Parent=CurrentParent;
            Node(CurrentNode).Children=ChildList{CurrentNode}; %This assumes directed graph
            Node(CurrentNode).IsEnd=isempty(ChildList{CurrentNode});
            Node(CurrentNode).Level=it;
            % Update Lists
            ToVisit(CurrentNode)=1;
        end
        Visited(CurrentParent)=1;
    end
    it=it+1;
end

%% Remove 
% Keep only nodes conected to Root
NodeVisited=find(ToVisit);
NodeRoot=Node(NodeVisited);
Nnode=length(NodeRoot);
for k=1:Nnode
    [~,kParent]=intersect(NodeVisited,NodeRoot(k).Parent);
    if(~isempty(kParent))
        NodeRoot(k).Parent=kParent;
    end
    [~,kChildren]=intersect(NodeVisited,NodeRoot(k).Children);
    NodeRoot(k).Children=kChildren;
end