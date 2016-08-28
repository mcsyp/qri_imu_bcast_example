
function Queue() {
    this.data=[];
    this.enqueue=enqueue;
    this.dequeue=dequeue;
    this.front=front;
    this.back=back;
    this.size=size;
}
function enqueue(element)
{
    this.data.push(element);
}
function dequeue(){
    this.data.shift();
}
function front(){
    return this.data[0];
}
function back(){
    return this.data[this.data.length-1];
}
function empty(){
    if(this.data.length===0){return true;}
    else{ return false;}
}
function size(){
    return this.data.length;
}
