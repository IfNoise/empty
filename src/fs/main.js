
var h = preact.h;
var App={};


App.Header = function(props) {
  return h(
      'div', {class: 'p-2 border-bottom bg-light'},
      h('b', {},' test1'),
      h('div', {class: 'float-right'},
        h('small', {class: 'text-muted mr-2 font-weight-light'},
          'mDash Smart Light'),
      ));
};
App.PageDeviceSettings=function(props){
  var self=this;
  self.componentDidMount=function(){

  }
}
App.Content=function (props) {
 return  h('h1',{},'"TEST CONTENT"');
}
App.Instance = function(props) {
  var self = this;
  App.self = self;
  self.componentDidMount=function(){
    var rpc = mkrpc('ws://' + location.host + '/rpc');
    self.r
  }

  self.render=function(props,state){
    return h(
      'div', {
        class: 'main border',
        style: 'max-width: 480px; margin: 0 auto; ' +
            'min-height: 100%; max-height: 100%;' +
            'display:grid;grid-template-rows: auto 1fr auto;' +
            'grid-template-columns: 100%;',
      },
      h(App.Header), h(App.Content));
  }
  return self.render(props);
}

  preact.render(h(App.Instance), document.body);
