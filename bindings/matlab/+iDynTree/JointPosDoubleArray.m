classdef JointPosDoubleArray < iDynTree.VectorDynSize
  methods
    function self = JointPosDoubleArray(varargin)
      self@iDynTree.VectorDynSize(SwigRef.Null);
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(870, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function varargout = resize(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(871, self, varargin{:});
    end
    function varargout = isConsistent(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(872, self, varargin{:});
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(873, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
