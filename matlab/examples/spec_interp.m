rand('seed',17);
%sequence lengths, number of sequences
len=100;
num_train=1000;
num_a=5;
shift=20;
cache=10;
use_sign=0;
normalization='NO'; %NO,SQRT,LEN,SQLEN,FULL
aa=(round(len/2-num_a/2)):(round(len/2+num_a/2-1));

%SVM regularization factor C
C=30;

%Weighted Degree kernel parameters
max_order=8;
order=8;

%generate some toy data
acgt='CCGT';
rand('state',1);
traindat=acgt(ceil(4*rand(len,num_train)));
trainlab=[-ones(1,num_train/2),ones(1,num_train/2)];
aas=floor((shift+1)*rand(num_train,1));
idx=find(trainlab==1);
for i=1:length(idx),
	traindat(aa+aas(i),idx(i))='A';
end
aas=floor((shift+1)*rand(num_train,num_a));
idx=find(trainlab==-1);
for i=1:floor(length(idx)/2),
	for j=1:num_a,
		traindat(aa(1)+aas(i,j),idx(i))='A';
	end
end
for i=floor(length(idx)/2)+1:length(idx),
	traindat(aa(1:3)+aas(i,1),idx(i))='AAA';
	traindat(aa(1)+1+aas(i,2),idx(i))='A';
end

%train svm
sg('send_command', 'use_linadd 1' );
sg('set_features', 'TRAIN', traindat, 'DNA');
sg('set_labels', 'TRAIN', trainlab);

%sg('send_command', sprintf('convert TRAIN STRING CHAR STRING WORD %i %i', order, order-1));
sg('send_command', sprintf('convert TRAIN STRING CHAR STRING WORD %i %i r', order, order-1));
sg('send_command', 'add_preproc SORTWORDSTRING') ;
sg('send_command', 'attach_preproc TRAIN') ;
%sg('send_command', sprintf('set_kernel COMMSTRING WORD %d %d %s',cache, use_sign, normalization));
sg('send_command', sprintf('set_kernel WEIGHTEDCOMMSTRING WORD %d %d %s',cache, use_sign, normalization));

sg('send_command', 'init_kernel TRAIN');
sg('send_command', 'new_svm LIGHT');
sg('send_command', sprintf('c %f',C));
sg('send_command', 'svm_train');
sg('send_command', 'init_kernel_optimization');

%evaluate svm on train data
sg('set_features', 'TEST', traindat, 'DNA');
%sg('send_command', sprintf('convert TEST STRING CHAR STRING WORD %i %i', order, order-1));
sg('send_command', sprintf('convert TEST STRING CHAR STRING WORD %i %i r', order, order-1));
sg('send_command', 'attach_preproc TEST') ;
sg('set_labels', 'TEST', trainlab);
sg('send_command', 'init_kernel TEST');
out=sg('svm_classify');
fprintf('accuracy: %f                                                                                         \n', mean(sign(out)==trainlab))
W=sg('get_SPEC_scoring', max_order);

x={};
X=zeros(max_order);
l=0;
for i=1:max_order,
	i
	L=l+4^i;
	x{i}=W((l+1):L);
	l=L;
	X(i)=max(abs(x{i}));
end

for i=1:max_order,
	figure(i)
	bar(x{i})
end

figure(99)
bar(X)
