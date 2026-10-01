function G = GraphAdapt(node,link,A)

%   G -> Graph
%       G.v -> Node positions
%       G.e -> Adjacency matrix
%       G.wv -> Node labels or attributes
%       G.we -> Edges labels or attributes --> access to second point of an edge between 1 and 5 nodes --> G.we{1,5}(2)
%       G.lp -> leaf points : points to the leaf if it is a endnode --> access to a first leaf point of the first leaf of the node 2 --> G.lp{2,1}(1)





if(isempty(A))
    G.e = A;
    G.v = zeros(size(node,2),3);
    G.lp = {};
    G.wv=zeros(size(node,2),1);
    G.we={};
    return;
end

G.e = A;
G.v = zeros(size(node,2),3);
G.lp{size(G.e,1),:} = [];
G.wv=zeros(size(node,2),1);


for i=1:size(node,2)
    G.v(i,:)=[node(i).comx,node(i).comy,node(i).comz]; %per dibuixar la y i la x van canviades
    if ~isempty(find(node(i).conn<=0, 1))
        G.wv(i)=0; %end node
        [ind] = find(node(i).conn<=0);
        for n=1:numel(ind)
            G.lp{i,n} =  link(node(i).links(ind(n))).point;
        end
    else
        G.wv(i)=1; % middle node
    end
    for j=1:length(node(i).links)
        for k=1:length(link(node(i).links(j)).point)
            if (link(node(i).links(j)).n1>0 && link(node(i).links(j)).n2>0)
                G.we{link(node(i).links(j)).n1,link(node(i).links(j)).n2}= [link(node(i).links(j)).point];
                G.we{link(node(i).links(j)).n2,link(node(i).links(j)).n1}= [link(node(i).links(j)).point];
            end
        end;
    end
end

G.wv(1) = -1; %root sempre és el inici de la traquea


end