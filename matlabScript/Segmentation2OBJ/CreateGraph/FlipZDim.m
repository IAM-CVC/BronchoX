function [zout]=FlipZDim(skel,z)
%flip dim of z coordinate
maxV=size(skel,3);
mid = maxV/2;
for i=1:size(z,1)
    if z(i)<mid
       zout(i) = z(i)+maxV-z(i)*2+1;
    elseif z(i)>mid
        zout(i) = z(i)-(z(i)-mid)*2+1;
    else
        zout(i)=z(i);
    end
end

end