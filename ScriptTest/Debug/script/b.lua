module(..., package.seeall);

function HelloWorldB()
    HelloWorldB1();
    HelloWorldB2();
end;

function HelloWorldB1()
    print("Hello World B1!");
end;

function HelloWorldB2()
    print("Hello World B2!");
end;