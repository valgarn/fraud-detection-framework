/*
 * Copyright 2021 The Fraud Detection Framework Authors.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND< either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

import React from 'react';
import Modal from 'react-modal';

import '../css/App.css';

import Config from '../config.js';
import Login from './Login';
import Dashboard from './Dashboard';

import Snackbar from '@material-ui/core/Snackbar';

/**
 * Main application class
 */
class App extends React.Component {

    /**
     * Style for main modal windows
     */
    static get modalStyle() {
        return {
            overlay: {
                position: 'fixed',
                top: 0,
                left: 0,
                right: 0,
                bottom: 0,
                zIndex: 10100,
                backgroundColor: 'rgba(0, 0, 0, 0.0)'
            },
            content: {
                top: '50%',
                left: '50%',
                right: 'auto',
                bottom: 'auto',
                marginRight: '-50%',
                transform: 'translate(-50%, -50%)',
                padding: '0px',
                overflow: 'hidden',
                backgroundColor: 'rgba(0, 0, 0, 0.0)'
            }
        }
    };

    /**
     * Constructor for the main application class
     * @param {*} props - constructor parameters
     */
    constructor(props) {

        super(props);

        this.state = {
            loginIsOpen: true,
            auth: '',
            message: ''
        };

        this.checkLogin = this.checkLogin.bind(this);
        this.setMessage = this.setMessage.bind(this);

        window.addEventListener("beforeunload", (ev) => {
            ev.preventDefault();
        });
    }

    /**
     * The event is invoked immediately before mounting occurs. 
     * It is called before render(), therefore setting state synchronously in this method will not trigger a re-rendering. 
     * Avoid introducing any side-effects or subscriptions in this method.
     */
    componentWillMount() {
        Modal.setAppElement('body');
    }

    /**
     * The event is invoked immediately after a component is mounted. 
     * Initialization that requires DOM nodes should go here. 
     * If you need to load data from a remote endpoint, this is a good place to instantiate the network request. 
     * Setting state in this method will trigger a re-rendering.
     */
    componentDidMount() {

    }

    checkLogin(nameValue, passwordValue) {
        this.state.auth = 'Basic ' + btoa(nameValue.toLowerCase() + ':' + passwordValue);
        let request = new Request(Config.apiUrl + 'login/');
        let headers = new Headers();
        headers.append('Accept', 'application/json');
        headers.append('Content-Type', 'application/json; charset=utf-8');
        headers.append('Authorization', this.state.auth);
        let params = {
            method: 'GET',
            headers: headers,
            mode: 'cors',
            cache: 'no-cache'
        };
        fetch(request, params)
            .then((response) => {
                if (response.ok) {
                    this.setState({ loginIsOpen: false });
                } else {
                    this.setMessage("Something wrong happened!");
                }
            })
            .catch((error) => {
                console.error(error);
                this.setMessage("Something wrong happened!");
            });
    }

    setMessage(s) {        
        this.setState({ message: s});
    }

    /**
     * The function is a root renderer
     */
    render() {

        if (this.state.loginIsOpen) {
            return (<div className="App">
                <Modal
                    contentLabel=''
                    isOpen={this.state.loginIsOpen}
                    style={App.modalStyle}
                    shouldCloseOnOverlayClick={true}
                    onRequestClose={this.closeImage}
                >
                    <Login
                        checkLogin={this.checkLogin}
                        innerRef={login => (this.login = login)}
                    />
                </Modal>
                <Snackbar 
                    open={!this.state.message==''}
                    message={this.state.message}
                    anchorOrigin={{
                        vertical: 'top',
                        horizontal: 'center',
                    }}
                    autoHideDuration={5000}
                    onClose={()=>{this.setState({message: ''})}}
                />
            </div>);
        } else {
            return (<div className="App">
                <Dashboard auth={this.state.auth}/>
            </div >);
        }
    }

}

export default App;
