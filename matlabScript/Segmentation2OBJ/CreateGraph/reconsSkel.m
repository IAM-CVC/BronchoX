%Print skel final
function [skel,xuniform,yuniform,zuniform] = reconsSkel(G,vol)
skel = zeros(size(vol));
xuniform=[];
yuniform=[];
zuniform=[];
%afegim nodes
for i=1:size(G.v,2)
    IND = round(sub2ind(size(vol),G.v(i,1),G.v(i,2),G.v(i,3)));
    skel(IND)=1;
end
%afegim arestes
for i=1:size(G.e,1)
    for j=1:size(G.e,2)
     if G.e(i,j)>0
        skel(G.we{i,j}(:))=1;
        
     end
    end
end
%afegim fulles
%adjNew = Graph2dirGraph(G.e);
%nodesSenseFills = find((sum(adjNew>0,2)==0)>0);
for i=1:size(G.e,1)
    for j=1:size(G.lp,2)       
        if ~isempty(G.lp{i,j})
            skel(G.lp{i,j}(:))=1;
        end      
    end
end


end