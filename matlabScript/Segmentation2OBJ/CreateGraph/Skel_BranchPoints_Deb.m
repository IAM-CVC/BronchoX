function [ link,skel,node,A ] = Skel_BranchPoints_Deb(skel,TH)

w = size(skel,1);
l = size(skel,2);
h = size(skel,3);

% convert skeleton to graph structure
[A,node,link] = Skel2Graph3D_Deb(skel,TH);
% convert graph structure back to (cleaned) skeleton
skel = Graph2Skel3D_Deb(node,link,w,l,h);

% % iteratively convert until there are no more 2-nodes left
% [A,node,link] = Skel2Graph3D(skel,TH);
% k=0;
% while(min(cellfun('length',{node.conn}))<3)
%     skel = Graph2Skel3D(node,link,w,l,h);
%     [A,node,link] = Skel2Graph3D(skel,TH);
%     k=k+1;
%     if(k>500) break; end
% end;


%   node=node(find((cellfun('length',{node.conn})<3)));
%   skel = Graph2Skel3D(node,link,w,l,h);
