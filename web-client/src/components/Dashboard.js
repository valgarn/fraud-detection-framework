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
import PropTypes from 'prop-types';
import classNames from 'classnames';
import { withStyles } from '@material-ui/core/styles';
import CssBaseline from '@material-ui/core/CssBaseline';
import Drawer from '@material-ui/core/Drawer';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Switch from '@material-ui/core/Switch';
import FormGroup from '@material-ui/core/FormGroup';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import FormControl from '@material-ui/core/FormControl';
import List from '@material-ui/core/List';
import Typography from '@material-ui/core/Typography';
import Divider from '@material-ui/core/Divider';
import IconButton from '@material-ui/core/IconButton';
import Badge from '@material-ui/core/Badge';
import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import CloudQueueIcon from '@material-ui/icons/CloudQueue';
import Avatar from '@material-ui/core/Avatar';
import LockOutlinedIcon from '@material-ui/icons/LockOutlined';
import Tooltip from '@material-ui/core/Tooltip';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableRow from '@material-ui/core/TableRow';
import Grid from '@material-ui/core/Grid';

import JSON5 from 'json5';

import Uppy from '@uppy/core';
import FileInput from '@uppy/file-input';
import XHRUpload from '@uppy/xhr-upload';
import ProgressBar from '@uppy/progress-bar';

import { sectionsListItems } from './listSections';

import '../css/Dashboard.css';
import LogoImage from '../images/penrose-white.png';
import Config from '../config.js';

const drawerWidth = 240;

const styles = theme => ({
    root: {
        display: 'flex',
    },
    toolbar: {
        paddingRight: 24,
    },
    toolbarIcon: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-end',
        padding: '0 8px',
        ...theme.mixins.toolbar,
    },
    appBar: {
        zIndex: theme.zIndex.drawer + 1,
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
    },
    appBarShift: {
        marginLeft: drawerWidth,
        width: `calc(100% - ${drawerWidth}px)`,
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.enteringScreen,
        }),
    },
    menuButton: {
        marginLeft: 12,
        marginRight: 36,
    },
    menuButtonHidden: {
        display: 'none',
    },
    title: {
        flexGrow: 1,
    },
    drawerPaper: {
        position: 'relative',
        whiteSpace: 'nowrap',
        width: drawerWidth,
        transition: theme.transitions.create('width', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.enteringScreen,
        }),
    },
    drawerPaperClose: {
        overflowX: 'hidden',
        transition: theme.transitions.create('width', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
        width: theme.spacing.unit * 7,
        [theme.breakpoints.up('sm')]: {
            width: theme.spacing.unit * 9,
        },
    },
    appBarSpacer: theme.mixins.toolbar,
    content: {
        flexGrow: 1,
        padding: theme.spacing.unit * 3,
        height: '100vh',
        overflow: 'auto',
    },
    chartContainer: {
        marginLeft: -22,
    },
    tableContainer: {
        height: 320,
    },
    h5: {
        marginBottom: theme.spacing.unit * 2,
    },
});

class Dashboard extends React.Component {

    static get photoStyle() {
        return {
            height: (window.innerHeight / 2) + "px",
            padding: "10px"
        }
    }

    static get commentConstants() {
        return [
            "ERROR_MISSING_FILE",
            "ERROR_OPENNING_FILE",
            "ERROR_CNN_MISSING_GRAPH_FILE",
            "ERROR_CNN_GRAPH_IMPORT",
            "ERROR_CNN_CREATE_SESSION",
            "ERROR_CNN_RUN_SESSION",
            "ERROR_BENFORD_JPEG_DECOMPRESSION",
            "ERROR_QUALITY_WRONG_JPEG",
            "ERROR_EXIF_NO"
        ]
    }

    constructor(props) {
        super(props);

        this.state = {
            identity: -1,
            intervalId: null,
            wait: "",
            photo: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVQYV2NgYAAAAAMAAWgmWQ0AAAAASUVORK5CYII=",
            progress: " ",
            comments: " ",
            queueSize: 0,
            resultColor: "white",
            result: "",
            open: true,

            infoAnalisis: false,
            statAnalisis: false,
            deepAnalisis: false,

            total: 0.0,

            info: [{
                Filename: '',
                Version: '',
                isBlur: '',
                isJpeg: '',
                Quality: '',
                Width: '',
                Height: '',
                Channels: '',
            }]
        };
        //this.waitResult = this.waitResult.bind(this);
    }

    waitResult(identity) {
        this.state.identity = identity;
        if (this.state.intervalId) {
            clearInterval(this.state.intervalId);
            this.state.intervalId = null;
        }
        fetch(Config.apiUrl + "get_photo/" + identity, {
            method: 'GET',
            headers: {
                'Authorization': this.props.auth
            }
        }
        ).then((response) => {
            if (response.ok) {
                return response.arrayBuffer();
            }
            console.log(response);
        }).then((blob) => {
            if (blob) {
                let data = new Buffer(blob, 'binary').toString('base64');
                this.setState({ photo: "data:image/*;base64," + data });
            }
        })
            .catch((error) => {
                console.error(error);
            });

        this.state.intervalId = setInterval(() => {
            fetch(Config.apiUrl + "get_result/" + identity, {
                method: 'GET',
                headers: {
                    'Authorization': this.props.auth
                }
            }
            ).then((response) => {
                if (response.ok) {
                    return response.json();
                }
                console.log(response);
            }).then((json) => {
                if (json && !json.status) {
                    clearInterval(this.state.intervalId);
                    this.state.intervalId = null;
                    let m = 0.0;
                    let comments = ""
                    let d = []
                    let seconds = 0;
                    Object.keys(json).forEach(function (key) {
                        let p = json[key];
                        seconds += p.processingSeconds;
                        d[key.toUpperCase()] = parseFloat(p.probability);
                        if (p.probability !== 0.5 && m < p.probability) {
                            m = p.probability;
                        }
                        if (p.comments && p.comments.length > 0) {
                            comments += p.comments + " ";
                        }
                    });
                    if (json.info) {
                        this.setState({ info: [JSON5.parse(json.info.comments)] });
                    }

                    let c = "red";
                    if (this.getAnalisisType() == "none") { 
                        this.setState({
                            result: [<b>No analysis methods have been selected!</b>,
                            <br></br>, <br></br>,
                            <i>Processing seconds: 0</i>]
                        });
                    } else {
                        c = "#" + Math.round(100 + (127 * m)).toString(16) + Math.round(100 + (127 * (1.0 - m))).toString(16) + "64";
                        this.setState({
                            result: [<b>Looks like the photo is {(m < 0.5 ? "NOT modified" : "MODIFIED")}</b>,
                            <br></br>, <br></br>,
                            <i>Processing seconds: {seconds.toFixed(2)}</i>]
                        });
                    }
                    if (comments && comments.length > 0) {
                        let isWrongBackend = false;
                        if (new RegExp(["ERROR_MISSING_FILE", "ERROR_OPENNING_FILE", "ERROR_CNN_MISSING_GRAPH_FILE",
                            "ERROR_CNN_GRAPH_IMPORT", "ERROR_CNN_CREATE_SESSION", "ERROR_CNN_RUN_SESSION"].join("|")).test(comments)) {
                            isWrongBackend = true;
                        }
                        let isBadJpeg = comments.indexOf("ERROR_BENFORD_JPEG_DECOMPRESSION") > -1 || comments.indexOf("ERROR_QUALITY_WRONG_JPEG") > -1;
                        let noExif = comments.indexOf("ERROR_EXIF_NO") > -1;
                        let lowJpegQuality = comments.indexOf("Bad JPEG quality") > -1;
                        let points = [];
                        if (isWrongBackend || isBadJpeg || noExif || lowJpegQuality) {
                            if (isWrongBackend) {
                                points.push("Some backend error happened. Notify author, please.");
                            }
                            if (isBadJpeg) {
                                points.push("The anti-fraud algorithms have optimized for jpeg photos. Use jpeg format if it is possible.");
                            }
                            if (noExif) {
                                points.push("Usually, each photo contains information about camera. Otherwise, the photo is suspicious.");
                            }
                            if (lowJpegQuality) {
                                points.push("It is recommended to use jpeg-photos with quality more 80, because jpeg artifacts can be recognized as fakes.");
                            }
                        }
                        comments = <span><ol>
                            {points.map(function (item, i) {
                                return <li key={i}>{item}</li>;
                            })}
                        </ol></span>;
                    }
                    this.setState({ resultColor: c, progress: s, comments: comments });
                    return;
                }
            })
                .catch((error) => {
                    console.error(error);
                });
            const s = this.state.progress + " . ";
            this.setState({ progress: s });
        }, 3000);
    }

    getAnalisisType() {
        let methods = [];
        if (this.state.infoAnalisis) {
            methods.push('info');
            methods.push('exif');
        }
        if (this.state.statAnalisis) {
            methods.push('benford');
            methods.push('pca');
        }
        if (this.state.deepAnalisis) {
            methods.push('quality');
            methods.push('cnn');
        }
        return methods.length > 0 ? methods.join('_') : 'none';
    }

    componentDidMount() {
        const { classes } = this.props;
        this.state.uppy = Uppy(
            {
                debug: true,
                autoProceed: true,
                lowercase: true,
                className: classes.button,
                restrictions: {
                    maxNumberOfFiles: 1,
                    allowedFileTypes: ['.jpg', '.jpeg', '.png', '.Jpg', '.Jpeg', '.Png', '.JPG', '.JPEG', '.PNG']
                },
                onBeforeUpload: (files) => {
                    const s = " ";
                    const c = "white";
                    const d = [
                        ['Element', 'Density', { role: 'style' }, { role: 'annotation' }],
                        ['Benford', 0.0, '#b87333', 0.0],
                        ['CNN', 0.0, '#b873ff', 0.0],
                        ['Quality', 0.0, 'gold', 0.0],
                        ['PCA', 0.0, 'silver', 0.0],
                        ['Exif', 0.0, 'color: #d5eef2', 0.0]
                    ];
                    const i = [{
                        Filename: '',
                        Version: '',
                        isBlur: '',
                        isJpeg: '',
                        Quality: '',
                        Width: '',
                        Height: '',
                        Channels: '',
                    }];
                    this.setState({
                        resultColor: c, progress: s, comments: s, data: d, info: i,
                        photo: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVQYV2NgYAAAAAMAAWgmWQ0AAAAASUVORK5CYII="
                    });
                    return files;
                }
            })
            .use(FileInput, {
                target: '.UppyDragDrop',
                inline: true,
                replaceTargetContent: true,
                locale: {
                    strings: {
                        chooseFiles: ' ⇦ select analysis methods and click this button for image uploading dialog'
                    },
                },
            })
            .use(XHRUpload, {
                endpoint: Config.apiUrl + 'set_photo/' + this.getAnalisisType(),
                headers: {
                    'Authorization': this.props.auth
                },
                formData: true,
                fieldName: 'photo'
            })
            .use(ProgressBar, {
                target: 'body',
                fixed: true,
                hideAfterFinish: true
            })
            .on('complete', (result) => {
                if (this.photoInput) {
                    this.state.uppy.reset();
                }
                if (result.successful.length === 1) {
                    this.waitResult(result.successful[0].response.body.identity);
                }
            });

        setInterval(() => {
            fetch(Config.apiUrl + "get_sources_queue_size/", {
                method: 'GET',
                headers: {
                    'Authorization': this.props.auth
                }
            }).then((response) => {
                if (response.ok) {
                    return response.json();
                }
                console.log(response);
            }).then((json) => {
                if (json) {
                    this.setState({ queueSize: json.size });
                }
            })
                .catch((error) => {
                    console.error(error);
                });
        }, 3000)
    }

    handleDrawerOpen = () => {
        this.setState({ open: true });
    };

    handleDrawerClose = () => {
        this.setState({ open: false });
    };

    handleAnalisis = name => event => {
        if (name == 'infoAnalisis') {
            this.state.infoAnalisis = event.target.checked;
        } else if (name == 'statAnalisis') {
            this.state.statAnalisis = event.target.checked;
        } else if (name == 'deepAnalisis') {
            this.state.deepAnalisis = event.target.checked;
        };
        this.state.uppy.plugins.uploader[0].opts.endpoint = Config.apiUrl + 'set_photo/' + this.getAnalisisType();
        this.setState({ [name]: event.target.checked, total: this.state.total + (event.target.checked ? 0.99 : -0.99) });
    };

    render() {
        const { classes } = this.props;
        let commentsStyle = {
            color: this.state.resultColor,
            marginLeft: "50px",
            width: "100%"
        };
        let resultStyle = {
            backgroundColor: this.state.resultColor,
        };
        let infoLabel = [<div>Informational</div>, <span style={{ color: "green" }}>FREE </span>,
        <span style={{ textDecoration: "line-through", textDecorationColor: "red" }}>$0.99</span>];
        let statLabel = [<div>Statistical</div>, <span style={{ color: "green" }}>FREE </span>,
        <span style={{ textDecoration: "line-through", textDecorationColor: "red" }}>$0.99</span>];
        let deepLabel = [<div>Deep dive</div>, <span style={{ color: "green" }}>FREE </span>,
        <span style={{ textDecoration: "line-through", textDecorationColor: "red" }}>$0.99</span>];
        return (
            <div className={classes.root}>
                <CssBaseline />
                <AppBar
                    position="absolute"
                    className={classNames(classes.appBar, this.state.open && classes.appBarShift)}
                >
                    <Toolbar disableGutters={!this.state.open} className={classes.toolbar}>
                        <IconButton
                            color="inherit"
                            aria-label="Open drawer"
                            onClick={this.handleDrawerOpen}
                            className={classNames(
                                classes.menuButton,
                                this.state.open && classes.menuButtonHidden,
                            )}
                        >
                            <MenuIcon />
                        </IconButton>
                        <Avatar src={LogoImage} style={{ borderRadius: 0, width: '50px' }}>
                            <LockOutlinedIcon />
                        </Avatar>
                        <Typography
                            component="h1"
                            variant="h6"
                            color="inherit"
                            noWrap
                            className={classes.title}
                        >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Fraud Detection Framework</Typography>
                        <Tooltip title="Queue size">
                            <IconButton color="inherit">
                                <Badge badgeContent={this.state.queueSize} color="secondary" showZero={true}>
                                    <CloudQueueIcon />
                                </Badge>
                            </IconButton>
                        </Tooltip>
                    </Toolbar>
                </AppBar>
                <Drawer
                    variant="permanent"
                    classes={{
                        paper: classNames(classes.drawerPaper, !this.state.open && classes.drawerPaperClose),
                    }}
                    open={this.state.open}
                >
                    <div className={classes.toolbarIcon}>
                        <IconButton onClick={this.handleDrawerClose}>
                            <ChevronLeftIcon />
                        </IconButton>
                    </div>
                    <Divider />
                    <List>{sectionsListItems}</List>
                    <Divider />

                    <Grid item xs={8} md={8} sm={8} lg={8}>
                        <Table>
                            {this.state.info.map((i) => {
                                return (
                                    <TableBody key={this.state.identity}>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>File name: {i.Filename}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>Blured: {i.isBlur.toString()}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>is JPEG: {i.isJpeg.toString()}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>Quality: {i.Quality}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>Width: {i.Width}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>Height: {i.Height}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>Channels: {i.Channels}</TableCell>
                                        </TableRow>
                                        <TableRow>
                                            <TableCell style={{ width: 'auto !important' }}>FDF version: {i.Version}</TableCell>
                                        </TableRow>
                                    </TableBody>
                                )
                            })}
                        </Table>
                        <div className="Result" style={resultStyle}>{this.state.result}</div>
                    </Grid>
                </Drawer>
                <main className={classes.content}>
                    <div className={classes.appBarSpacer} />
                    <FormControl component="fieldset">
                        <FormGroup aria-label="position" row>
                            <FormControlLabel
                                value="info_exif"
                                control={<Switch size="small" color="primary" checked={this.state.infoAnalisis} onChange={this.handleAnalisis('infoAnalisis')} ref={(infoAnalisis) => this.infoAnalisis = infoAnalisis} />}
                                label={infoLabel}
                                labelPlacement="top"
                            />
                            <FormControlLabel
                                value="benfor_pca"
                                control={<Switch size="small" color="primary" checked={this.state.statAnalisis} onChange={this.handleAnalisis('statAnalisis')} ref={(statAnalisis) => this.statAnalisis = statAnalisis} />}
                                label={statLabel}
                                labelPlacement="top"
                            />
                            <FormControlLabel
                                value="quality_cnn"
                                control={<Switch size="small" color="primary" checked={this.state.deepAnalisis} onChange={this.handleAnalisis('deepAnalisis')} ref={(deepAnalisis) => this.deepAnalisis = deepAnalisis} />}
                                label={deepLabel}
                                labelPlacement="top"
                            />
                            <span className="PhotoInput" ref={(photoInput) => this.photoInput = photoInput}>
                                <div className="UppyDragDrop"></div>
                            </span>
                            <span style={{ padding: "20px", textAlign: "center" }}>
                                <div>TOTAL</div>
                                <span style={{ color: "green" }}>FREE </span>
                                <span style={{ textDecoration: "line-through", textDecorationColor: "red" }}>${this.state.total.toFixed(2)}</span>
                            </span>
                        </FormGroup>
                    </FormControl>
                    <br /><br /><div className="PhotoInput" style={commentsStyle}>{this.state.comments}</div>
                    <img alt="Your photo" style={Dashboard.photoStyle} src={this.state.photo} />
                </main>
            </div >
        );
    }
}

Dashboard.propTypes = {
    classes: PropTypes.object.isRequired,
};

export default withStyles(styles)(Dashboard);
