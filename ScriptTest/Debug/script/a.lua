module(..., package.seeall);

function HelloWorldA()
    HelloWorldA1();
    print("------")
    b.HelloWorldB1();
end;

function HelloWorldA1()
    ServerCommon.MyTestFunc1();
end;

function HelloWorldA2(iParam, sParam)
    ServerCommon.MyTestFunc2();
    iParam = iParam * 2;
    print(""..iParam);
    print(sParam);
end;