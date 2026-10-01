function printSkel(skel, color)


%figure,
w=size(skel,1);
l=size(skel,2);
h=size(skel,3);
[x,y,z]=ind2sub([w,l,h],find(skel(:)));
plot3(y,x,z,'o','Markersize',2,...
     'MarkerFaceColor',color,...
     'Color',color);
%hold on,
axis equal;

end