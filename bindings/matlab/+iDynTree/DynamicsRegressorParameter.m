classdef DynamicsRegressorParameter < SwigRef
  methods
    function this = swig_this(self)
      this = iDynTreeMEX(3, self);
    end
    function varargout = category(self, varargin)
      narginchk(1, 2)
      if nargin==1
        nargoutchk(0, 1)
        varargout{1} = iDynTreeMEX(1121, self);
      else
        nargoutchk(0, 0)
        iDynTreeMEX(1122, self, varargin{1});
      end
    end
    function varargout = elemIndex(self, varargin)
      narginchk(1, 2)
      if nargin==1
        nargoutchk(0, 1)
        varargout{1} = iDynTreeMEX(1123, self);
      else
        nargoutchk(0, 0)
        iDynTreeMEX(1124, self, varargin{1});
      end
    end
    function varargout = type(self, varargin)
      narginchk(1, 2)
      if nargin==1
        nargoutchk(0, 1)
        varargout{1} = iDynTreeMEX(1125, self);
      else
        nargoutchk(0, 0)
        iDynTreeMEX(1126, self, varargin{1});
      end
    end
    function varargout = lt(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1127, self, varargin{:});
    end
    function varargout = eq(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1128, self, varargin{:});
    end
    function varargout = ne(self,varargin)
      [varargout{1:nargout}] = iDynTreeMEX(1129, self, varargin{:});
    end
    function self = DynamicsRegressorParameter(varargin)
      if nargin==1 && strcmp(class(varargin{1}),'SwigRef')
        if ~isnull(varargin{1})
          self.swigPtr = varargin{1}.swigPtr;
        end
      else
        tmp = iDynTreeMEX(1130, varargin{:});
        self.swigPtr = tmp.swigPtr;
        tmp.swigPtr = [];
      end
    end
    function delete(self)
      if self.swigPtr
        iDynTreeMEX(1131, self);
        self.swigPtr=[];
      end
    end
  end
  methods(Static)
  end
end
