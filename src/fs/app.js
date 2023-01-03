var h = preact.h;
var App = {};

App.settings = {

};

App.state = {
  connected: false,
  outputs: [],
  sensors: [],
  config: {}
};

App.Header = function (props) {
  return h(
    'div', { class: 'p-2 border-bottom bg-light' },
    h('b', {}, props.app.state.title),
    h('div', { class: 'float-right' },
      h('small', { class: 'text-muted mr-2 font-weight-light' },
        'Websocket connected:'),
      h('b', {}, props.app.state.connected ? 'yes' : 'no')));
};

App.Footer = function (props) {
  var self = this, app = props.app;

  var mkTabButton = function (title, icon, tab, href) {
    var active = (location.hash == href.replace(/.*#/, '#'));
    return h(
      'a', {
      href: href,
      class: 'text-center ' + (active ? 'font-weight-bold text-primary' : 'text-dark'),
      style: 'flex:1;height:3em;text-decoration:none;' + 'border-top: 3px solid ' + (active ? '#007bff' : 'transparent')
    },
      h('div', { class: '', style: 'line-height: 1.4em' },
        h('i', { class: 'mr-0 fa-fw fa ' + icon, style: 'width: 2em;' }),
        h('div', { class: 'small' }, title)));
  };

  return h(
    'footer', {
    class: 'd-flex align-items-stretch border-top',
    style: 'flex-shrink: 0;'
  },
    mkTabButton('Dashboard', 'fa-server', App.PageDashboard, '#/'),
    mkTabButton('Settings', 'fa-gear', App.PageSettings, '#/config'),
    mkTabButton('Files', 'fa-folder', App.PageDeviceFiles, '#/files'));
};

App.errorHandler = function (e) {
  var o = ((e.response || {}).data || {}).error || {};
  alert(o.message || e.message || e);
};

App.setKey = function (obj, key, val) {
  var parts = key.split('.');
  console.log(val);
  console.log(obj);
  console.log(key);
  for (var i = 0; i < parts.length; i++) {
    if (i >= parts.length - 1) {
      obj[parts[i]] = val;
    } else {
      if (!obj[parts[i]]) obj[parts[i]] = {};
      obj = obj[parts[i]];
    }
  }
};

App.getKey = function (obj, key) {
  var parts = key.split('.');
  for (var i = 0; i < parts.length; i++) {
    if (typeof (obj) != 'object') return undefined;
    if (!(parts[i] in obj)) return undefined;
    obj = obj[parts[i]];
  }
  return obj;
};




App.PageSettings = function (props) {
  const self = this;
  self.componentDidMount = function () {
    props.app.setState({ title: 'Settings' });
    self.setState({ changes: { config: null } });
    self.setState({ modifed: false });
  };

  const mkStringItem = function (label, k, dis, c, path, dst) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'text',
        // value: state.c[k] || r.config[k] || '',
        value: App.getKey(c, k),
        placeholder: App.getKey(c, k),
        disabled: dis,
        class: 'form-control',
        onInput: function (ev) {
          App.setKey(dst, path, ev.target.value);
          App.setKey(props.app.state, path, ev.target.value);
          self.setState({ modifed: true });
        },
      })));
  };
  const mkIntItem = function (label, k, dis, c, path, dst) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'number',
        step: 1,
        // value: state.c[k] || r.config[k] || '',
        value: App.getKey(c, k),
        placeholder: App.getKey(c, k),
        disabled: dis,
        class: 'form-control',
        onInput: function (ev) {
          console.log(ev);
          App.setKey(dst, path, parseInt(ev.target.value));
          App.setKey(props.app.state, path, parseInt(ev.target.value));
          self.setState({ modifed: true });
        },
      })));
  };

  const mkFloatItem = function (label, k, dis, c, path, dst) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'number',
        step: 0.1,
        value: App.getKey(c, k),
        placeholder: App.getKey(c, k),
        disabled: dis,
        class: 'form-control',
        onInput: function (ev) {
          console.log(ev);

          App.setKey(dst, path, parseFloat(ev.target.value));
          App.setKey(props.app.state, path, parseFloat(ev.target.value));
          self.setState({ modifed: true });
        },
      })));
  };
  const mkBoolItem = function (label, k, dis, c, path, dst) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'checkbox',
        value: App.getKey(c, k),
        checked: c,
        disabled: dis,
        class: 'form-control',
        onChange: function (ev) {
          App.setKey(dst, path, ev.target.checked);
          App.setKey(props.app.state, path, ev.target.checked);
          self.setState({ modifed: true });
          console.log(dst);

        },
      })));
  };
  const mkchap = function (label, c, p) {
    var items = Object.keys(c);

    return h(
      'div', { class: 'd-block  p-2' },
      h('h2', {}, label), items.map(function (k) {
        const par = p + '.' + k;
        const type = typeof c[k];
        if (type === 'object') {
          return mkchap(k, c[k], par);
        } else if (type === 'string') {
          return mkStringItem(k, k, false, c, par, self.state.changes)
        } else if (type === 'number') {
          return mkIntItem(k, k, false, c, par, self.state.changes)
        } else if (type === 'boolean') {
          return mkBoolItem(k, k, false, c[k], par, self.state.changes)
        }
      }));

  }

  const saveBtn = function (config) {
    return h('button',
      {
        class: 'btn btn-success float-right',
        disabled: !self.state.modifed,
        onClick: () => {
          if (self.state.modifed) {
            props.app.rpc.call('Config.Set', self.state.changes, 50000)
              .then(res => {
                if (res.result) {
                  console.log(res.result);
                  props.app.rpc.call('Config.Save', {}, 50000).then(res => {
                    console.log(res.result)
                  })
                  self.setState({ changes: { config: null } });
                  self.setState({ modifed: false });
                }
              }).catch(err => {
                console.log(err);
              })
          };
        }
      }, 'Save');
  }
  const rstBtn = function () {
    return h('button', {
      class: 'btn btn-danger float-right',
      onClick: (e) => {
        self.setState({ changes: { config: null } });
        self.setState({ modifed: false });
      }
    }, 'Reset');
  }

  self.render = function (props, state) {
    var chapters = Object.keys(props.app.state.config);
    return h(
      'div', {},
      h('div', { class: 'conf-page scroll items-align-stretch ' }, chapters ?
        chapters.map(function (k) {

          return mkchap(k, props.app.state.config[k], 'config.' + k);

        }) : ''),
      h('div', { class: 'd-block w-100' },
        h('div', { class: 'd-block chng-box ' }, self.state.modifed ? JSON.stringify(self.state.changes) : ''),
        rstBtn(),
        saveBtn()));
  }
};

App.Regulator = function (props) {
  let self = this;
  const app = props.app;
  const obj = app.state.config[props.obj];
  self.componentDidMount = function () {
    self.setState({ changes: { config: {} } });
    self.setState({ modifed: false });
    console.log('Regulator mounted')
    console.log(obj)

  };
  const mkSensSel = () => {
    const sens = app.state.sensors;
    return h('select', {
      class: 'input-selector col m-1',
      value: App.getKey(obj, 'input'),
      onChange: (ev) => {
        self.setState({ changes: { config: { [props.obj]: { input: ev.target.value } } } });
        App.setKey(app.state.config[props.obj], 'input', ev.target.value);
        self.setState({ modifed: true });
        console.log(self.state.modifed);

      }
    },
      sens.map((s) => {
        return h('option', {}, s.name)
      }));
  }
  const mkOutSel = () => {
    const outs = app.state.outputs;
    return h('select', {
      class: 'output-selector col',
      value: App.getKey(obj, 'output'),
      onChange: (ev) => {
        self.setState({ changes: { config: { [props.obj]: { output: ev.target.value } } } });
        App.setKey(obj, 'output', ev.target.value);
        self.setState({ modifed: true });
      }
    },
      outs.map((o) => {
        return h('option', {}, o.name)
      }));

  }
  const mkFloatItem = function (label, k, c) {
    return h(
      'div', { class: 'form-group row dash-input ' },
      h('label', { class: 'input-label col' }, label),
      h('input', {
        type: 'number',
        class: 'float-input col',
        step: 0.1,
        value: App.getKey(c, k),
        //placeholder: App.getKey(c, k),
        onInput: function (ev) {
          //App.setKey(self.state.changes.config[props.obj], k, parseFloat(ev.target.value));
          self.setState({ changes: { config: { [props.obj]: { [k]: parseFloat(ev.target.value) } } } });
          App.setKey(c, k, parseFloat(ev.target.value));
          self.setState({ modifed: true });
        },
      }));
  };
  const mkBoolItem = function (label, k, c,) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col m-0 ' }, label),
      h('div', { class: 'col' }, h('input', {
        type: 'checkbox',
        // value: App.getKey(c, k),
        checked: App.getKey(c, k),
        class: 'input-checkbox',
        onChange: function (ev) {
          //App.setKey(dst, path, ev.target.checked);
          self.setState({ changes: { config: { [props.obj]: { [k]: ev.target.checked } } } });
          App.setKey(c, k, ev.target.checked);
          self.setState({ modifed: true });
        },
      })));
  };

  const mkVDisp = () => {

    return h('div', { class: 'd-block' },
      h('div', { class: 'value-display' }, !app.state.device.state ? h('small', { class: 'text-muted mr-2 font-weight-light' }, 'Loading') : app.state.device.state.sensors.find((item) => { return item.name == App.getKey(obj, 'input') }).state + '°C'))
  }
  const indicator = function () {
    const state = app.state.device.state.outputs.find((item) => { return item.name == App.getKey(obj, 'output') }).state;
    return h('i', { class: 'fa fa-solid indicator fa-power-off ', style: 'color:' + (state ? 'lime' : 'grey') })
  }

  const saveBtn = function () {
    return h('button',
      {
        class: 'btn btn-success m-1 col-4',
        disabled: !self.state.modifed,
        onClick: () => {
          if (self.state.modifed) {
            app.rpc.call('Config.Set', self.state.changes, 50000)
              .then(res => {
                if (res.result) {
                  console.log(res.result);
                  app.rpc.call('Config.Save', {}, 50000).then(res => {
                    console.log(res.result)
                  })
                  self.setState({ changes: { config: {} } });
                  self.setState({ modifed: false });
                  app.loadConfig();
                }
              }).catch(err => {
                console.log(err);
              })
          };
        }
      }, 'Save');
  }
  const rstBtn = function () {
    return h('button', {
      class: 'btn btn-danger m-1 col-4',
      disabled: !self.state.modifed,
      onClick: (e) => {
        self.setState({ changes: { config: null } });
        self.setState({ modifed: false });
      }
    }, 'Reset');
  }

  return h('div', { class: 'regulator ' },
    h('div', { class: 'head row' },
      h('div', { style: 'font-size: 18pt' }, App.getKey(obj, 'name')),
      mkBoolItem('enable', 'enable', obj),
      indicator()),

    !(app.state.loaded || app.state.device.state) ? h('small', { class: 'text-muted mr-2 font-weight-light' }, 'Loading') :
      h('div', { class: 'row' }, mkSensSel(), mkOutSel()),
    mkVDisp(),
    mkFloatItem('Setpoint °C', 'setpoint', obj),
    mkFloatItem('Histeresis °C', 'hist', obj),
    h('code', { class: ' chng-box ' }, self.state.modifed ? JSON.stringify(self.state.changes) : ''),
    h('div', { class: 'row ' }, saveBtn(), rstBtn())
  )
}
App.LightTimer = function (props) {
  let self = this;
  const app = props.app;
  const obj = app.state.config[props.obj];
  self.componentDidMount = function () {
    self.setState({ changes: { config: {} } });
    self.setState({ modifed: false });
    console.log('Regulator mounted')
    console.log(obj)

  };
  var mkTimeItem = function (label, k, c) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'time',
        step: 60,
        // value: state.c[k] || r.config[k] || '',
        value: App.getKey(c, k),
        placeholder: App.getKey(c, k),
        class: 'time-input',
        onInput: function (ev) {
          const hm = ev.target.value.split(':')
          const hours = hm[0];
          const minute = hm[1];
          const int = hours * 60 + minute;
          self.setState({ changes: { config: { [props.obj]: { [k]: parseFloat(ev.target.value) } } } });
          App.setKey(c, k, int);
          self.setState({ modifed: true });

        },
      })))
  }
  const mkOutSel = () => {
    const outs = app.state.outputs;
    return h('select', {
      class: 'output-selector col',
      value: App.getKey(obj, 'output'),
      onChange: (ev) => {
        self.setState({ changes: { config: { [props.obj]: { output: ev.target.value } } } });
        App.setKey(obj, 'output', ev.target.value);
        self.setState({ modifed: true });
      }
    },
      outs.map((o) => {
        return h('option', {}, o.name)
      }));

  }
  return h('div', { class: 'regulator ' },
    h('h3', {}, App.getKey(obj, 'name')),
    !app.state.loaded ? h('small', { class: 'text-muted mr-2 font-weight-light' }, 'Loading') :
      mkOutSel(app),
    mkTimeItem('Start:', 'start', false, obj),
    mkTimeItem('Stop:', 'stop', false, obj))
};
App.IrrigationTimer = function (props) {
  let self = this;
  const app = props.app;
  const obj = app.state.config[props.obj];
  self.componentDidMount = function () {
    self.setState({ changes: { config: {} } });
    self.setState({ modifed: false });

  };
  const mkIntItem = function (label, k, c, ind) {
    return h(
      'div', { class: 'form-group row dash-input ' },
      h('label', { class: 'input-label col' }, label),
      h('input', {
        type: 'number',
        class: 'float-input col',
        step: 1,
        value: App.getKey(c, k)/ind,
        //placeholder: App.getKey(c, k),
        onInput: function (ev) {
          //App.setKey(self.state.changes.config[props.obj], k, parseFloat(ev.target.value));
          self.setState({ changes: { config: { [props.obj]: { [k]: (parseInt(ev.target.value)*ind) } } } });
          App.setKey(c, k, (parseInt(ev.target.value)*ind));
          App.setKey(props.app.state, k, (parseInt(ev.target.value)*ind));
          self.setState({ modifed: true });
        },
      }));
  };
  var mkTimeItem = function (label, k, c) {
    return h(
      'div', { class: 'form-group row my-2' },
      h('label', { class: 'col-form-label col-4' }, label),
      h('div', { class: 'col-8' }, h('input', {
        type: 'time',
        step: 60,
        // value: state.c[k] || r.config[k] || '',
        value: App.getKey(c, k),
        placeholder: App.getKey(c, k),
        class: 'time-input',
        onInput: function (ev) {
          const hm = ev.target.value.split(':')
          const hours = hm[0];
          const minute = hm[1];
          const int = hours * 3600 + minute*60;
          self.setState({ changes: { config: { [props.obj]: { [k]: int } } } });
          App.setKey(c, k, int);
          self.setState({ modifed: true });

        },
      })))
  }
  const indicator = function () {
    const state = app.state.device.state.outputs.find((item) => { return item.name === App.getKey(obj, 'output') }).state;
    return h('i', { class: 'fa fa-solid indicator fa-power-off ', style: 'color:' + (state ? 'lime' : 'grey') })
  }

  const saveBtn = function () {
    return h('button',
      {
        class: 'btn btn-success m-1 col-4',
        disabled: !self.state.modifed,
        onClick: () => {
          if (self.state.modifed) {
            app.rpc.call('Config.Set', self.state.changes, 50000)
              .then(res => {
                if (res.result) {
                  console.log(res.result);
                  app.rpc.call('Config.Save', {}, 50000).then(res => {
                    console.log(res.result)
                  })
                  self.setState({ changes: { config: {} } });
                  self.setState({ modifed: false });
                  app.loadConfig();
                }
              }).catch(err => {
                console.log(err);
              })
          };
        }
      }, 'Save');
  }
  const rstBtn = function () {
    return h('button', {
      class: 'btn btn-danger m-1 col-4',
      disabled: !self.state.modifed,
      onClick: (e) => {
        self.setState({ changes: { config: null } });
        self.setState({ modifed: false });
      }
    }, 'Reset');
  }

  const mkOutSel = () => {
    const outs = app.state.outputs;
    return h('select', {
      class: 'output-selector col',
      value: App.getKey(obj, 'output'),
      onChange: (ev) => {
        self.setState({ changes: { config: { [props.obj]: { output: ev.target.value } } } });
        App.setKey(obj, 'output', ev.target.value);
        self.setState({ modifed: true });
      }
    },
      outs.map((o) => {
        return h('option', {}, o.name)
      }));

  }

  return h('div', { class: 'regulator' },

    !app.state.loaded ? h('small', { class: 'text-muted mr-2 font-weight-light' }, 'Loading') :
      h('div', { class: 'head row' },
        h('div', { style: 'font-size: 18pt' }, App.getKey(obj, 'name')),
        indicator()),
    mkOutSel(app),
    mkTimeItem('Start:', 'start', false, obj),
    mkTimeItem('Stop:', 'stop', false, obj),
    mkIntItem('Number of Irr', 'num', obj,1),
    mkIntItem('Irr. window', 'win', obj,60),
    h('code', { class: ' chng-box ' }, self.state.modifed ? JSON.stringify(self.state.changes) : ''),
    h('div', { class: 'row ' }, saveBtn(), rstBtn())
  )
}
App.PageDashboard = function (props) {
  const self = this;
  self.componentDidMount = function () {
    props.app.setState({ title: 'Dashboard' });
  };

  self.render = function (props, state) {

    return h('div',
      { class: 'w-100 overflow-auto p-2 text-muted font-weight-light', style: 'scroll' }, !props.app.state.loaded ? h('h1', {}, 'Loading') :
      h(App.Regulator, { app: props.app, obj: 'reg1' }),
      h(App.Regulator, { app: props.app, obj: 'reg2' }),
      h(App.Regulator, { app: props.app, obj: 'reg3' }),
      h(App.LightTimer, { app: props.app, obj: 'light1' }),
      h(App.IrrigationTimer, { app: props.app, obj: 'irr1' })
    );
  }
};


App.PageDeviceFiles = function (props) {
  var self = this;
  self.state.files = [];

  self.loadFiles = async function () {
    const res = await props.app.rpc.call('FS.ListExt', {}, 60000);
    self.setState({ files: res.result });
    self.setState({ loaded: true });
    console.log(self.state.files)
    return res.result;
  }
  self.openFile = async function (file) {
    const res = await props.app.rpc.call('FS.Get', { filename: file }, 60000);
    const win = window.open('about:blank', '_blank');
    let text = atob(res.result.data);
    win.document.write(text);
    win.focus();
  }

  self.componentDidMount = function () {
    props.app.setState({ title: 'Files' });
    self.loadFiles().then(r => console.log(r));
  };
  const mkFileItem = (f) => {
    return h('tr',
      {
        class: '',
        onDblClick: (ev) => {
          self.openFile(f.name);
        }
      },
      h('td', {}, h('i', { class: 'col-sm fa fa-file' })),
      h('td', {}, f.name),
      h('td', {}, f.size))
  }
  self.render = (props, state) => h('div',
    { class: 'w-100 text-muted font-weight-light', style: 'scroll' }, !self.state.loaded ? h('h1', {}, 'Loading') :
    //h('i', { class: 'fa fa-folder' }), h('label', { class: 'p-0' }, 'Device Files'),
    h('table', { class: 'table table-hover' },
      //h('thead',{class:'thead-dark'},h('tr',{},h('th',{scope:'col'},' '),h('th',{scope:'col'},'name'),h('th',{scope:'col'},'size'))),
      h('tbody', {},
        self.state.files.map(file => mkFileItem(file))))
  );
};

App.PageDeviceState = function (props) {
  var self = this;

  self.componentDidMount = function () {

  };



};

App.Content = function (props) {
  return h(
    preactRouter.Router, {
    history: History.createHashHistory(),
    onChange: function (ev) {
      props.app.setState({ url: ev.url });
    }
  },
    h(App.PageDashboard, { app: props.app, default: true }),
    h(App.PageSettings, { app: props.app, path: 'config' }),
    h(App.PageDeviceFiles, { app: props.app, path: 'files' }),
    h(App.PageDeviceState, { app: props.app, path: 'state' }));

};

App.Instance = function (props) {
  var self = this;
  App.self = self;
  self.updateDeviceState = function () {

    if (self.state.loaded) {
      self.rpc.call('Get.State', {}, 50000)
        .then(res => {
          self.setState({ device: { state: res.result } });
          console.log(res.result);
        }).catch(err => {
          console.log(err);
        });
    }
  };
  self.componentDidMount = function () {
    self.setState({ device: { state: {} } });
    const loadSensors = function () {
      return self.rpc.call('Get.Sensors', {}, 50000)
        .then(res => {
          self.setState({ sensors: res.result.sensors });
          console.log(res);
        });
    }
    const loadConfig = function () {
      return self.rpc.call('Config.Get', {}, 50000)
        .then(res => {
          self.setState({ config: res.result });
          console.log(res);
          //
        })
    }
    const loadOutputs = function () {
      return self.rpc.call('Get.Outputs', {}, 50000)
        .then(res => {
          self.setState({ outputs: res.result.outputs });
        }).catch(err => {
          console.log(err);
        });
    }



    // Setup JSON-RPC engine
    var rpc = mkrpc('ws://' + '192.168.2.160' + '/rpc');
    rpc.onopen = ev => {
      // When RPC is connected, fetch list of supported RPC services
      self.setState({ connected: true });

      loadConfig().then(() => loadSensors()).then(() => loadOutputs()).then(() => {
        if (self.state.config) {
          self.setState({ loaded: true });
          console.log('All data loaded');
        } else {
          console.log('Some wents wrong');
        }

      })

    };
    rpc.onclose = ev => self.setState({ connected: false });
    //rpc.onout = ev => logframe('-> ', ev);
    rpc.onin = ev => console.log('<- ', ev);
    self.rpc = rpc;
    self.timer = setInterval(() => {
      self.updateDeviceState();
    }, 5000);
  };


  self.render = function (props, state) {
    var p = { app: self };
    if (!self.state.loaded) return h('div', { style: 'font-size:30pt' }, 'Loading...');    // Show blank page when loading
    //  if (!self.state.u) return h(App.Login, p);  // Show login unless logged
    return h(
      'div', {
      class: 'main border',
      style: 'max-width: 480px; margin: 0 auto; ' +
        'min-height: 100%; max-height: 100%;' +
        'display:grid;grid-template-rows: auto 1fr auto;' +
        'grid-template-columns: 100%;',
    },
      h(App.Header, p), h(App.Content, p), h(App.Footer, p));
  };

  return self.render(props, self.state);
};

window.onload = function () {
  if (!window.localStorage) alert('Unsupported platform!');
  preact.render(h(App.Instance), document.body);

  if ('serviceWorker' in navigator)  // for PWA
    navigator.serviceWorker.register('service-worker.js')
      .catch(function (err) { });
};
