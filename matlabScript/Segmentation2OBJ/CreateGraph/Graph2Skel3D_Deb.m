function skel = Graph2Skel3D_Deb(node,link,w,l,h)

% create binary image
skel = false(w,l,h);

% for all links
for i=1:length(link)
    
    skel(link(i).point)=1; % link voxels
        
end;

% add nodes
for i=1:length(node)
    
    skel(node(i).idx)=1; % link voxels
        
end;
