function [ ] = writePoint_txt( node,FileOut )

NPts=size(node,1);
fid=fopen(FileOut,'w');
for k=1:NPts
    fprintf(fid,'%.2f %.2f %.2f\n',node(k,1),node(k,2),node(k,3)); 
end
fid=fclose(fid);
end

