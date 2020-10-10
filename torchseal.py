import torchsealcpp as t

class Module:
    def __init__(self):
        t.init()
        pass

    def __call__(self, x):
        return self.forward(x)

    def forward(self, x):
        return x


class Sequential:
    def __init__(self, *args):
        self.args = args

    def __call__(self, x):
        return self.forward(x)

    def forward(self, x):
        for arg in self.args:
            x = arg.forward(x)
        return x

class Conv2d:
    def __init__(self, in_ch, out_ch, size, stride):
        self.window = t.Tensor4(out_ch, in_ch, size, size)
        self.stride = stride
        t.fill4(self.window)

    def forward(self, x):
        return t.conv(x, self.window, self.stride)

class Linear:
    def __init__(self, in_ch, out_ch):
        self.weight = t.Tensor2(in_ch, out_ch)
        t.fill2(self.weight)

    def forward(self, x):
        return t.fc(x, self.weight)

class Square:
    def __init__(self):
        pass

    def forward(self, x):
        if type(x) is t.Tensor1:
            return t.square1(x)
        elif type(x) is t.Tensor3:
            return t.square3(x)

def flatten(x):
    return t.flatten(x)
