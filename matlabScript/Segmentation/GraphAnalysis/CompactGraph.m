function G = CompactGraph(G)
%function to reduce prunned graph to a graph with the esential nodes
%   G -> Graph 
%       G.v -> Node positions
%       G.e -> Adjacency matrix
%       G.wv -> Node labels or attributes 
%       G.we -> Edges labels or attributes --> access to second point of an edge between 1 and 5 nodes --> G.we{1,5}(2)
%       G.lp -> leaf points : points to the leaf if it is a endnode --> access to a first leaf point of the first leaf of the node 2 --> G.we{2,1}(1)

%find the final nodes (componen with maximal nodes)
%create a new graph with final nodes
[indComp, nnodes] = components(sparse(G.e));
[val,indx] = max(nnodes);
nodes = find(indComp==indx);
G.e = G.e(nodes,nodes);
G.v = G.v(nodes,:);
G.wv = G.wv(nodes);
p={};q={};
indxi = 1; indxj=1;
for i=1:numel(nodes)
  for n=1:size(G.lp,2)
    q{indxi,n} = G.lp{nodes(i),n};
  end
  for j=1:numel(nodes)
    p{indxi,indxj} = [G.we{nodes(i),nodes(j)}];
    indxj=indxj+1;
  end
  indxj=1;
  indxi=indxi+1;
end
G.we=p; 
G.lp=q;
%G = AirwayProperties(G);
if(isfield(G,'rootNode'))
   G.rootNode=find(nodes==G.rootNode);
   if(isempty(G.rootNode))
       G.rootNode=1;
   end
end
end