--
-- PostgreSQL database dump
--

-- Dumped from database version 10.10
-- Dumped by pg_dump version 10.10

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- Name: add_message(text, bigint, bigint); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.add_message(message_value text, type_id_value bigint, user_id_value bigint DEFAULT NULL::bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $$

DECLARE 
        message_id_value bigint;

BEGIN
	INSERT INTO messages (message, user_id, type_id)
    	VALUES (message_value, user_id_value, type_id_value)
        RETURNING id INTO message_id_value;
	RETURN message_id_value;
END

$$;


ALTER FUNCTION public.add_message(message_value text, type_id_value bigint, user_id_value bigint) OWNER TO postgres;

--
-- Name: add_source(text, bigint, bigint, text); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.add_source(destination_value text, user_id_value bigint DEFAULT NULL::bigint, type_id_value bigint DEFAULT NULL::bigint, info_value text DEFAULT NULL::text) RETURNS bigint
    LANGUAGE plpgsql
    AS $$

DECLARE 
        source_id_value bigint;

BEGIN
	INSERT INTO sources (destination, user_id, type_id, info)
    	VALUES (destination_value, user_id_value, type_id_value, info_value)
        RETURNING id INTO source_id_value;
	RETURN source_id_value;
END

$$;


ALTER FUNCTION public.add_source(destination_value text, user_id_value bigint, type_id_value bigint, info_value text) OWNER TO postgres;

--
-- Name: check_login(text, text); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.check_login(name_value text, password_value text) RETURNS TABLE(user_id bigint)
    LANGUAGE plpgsql ROWS 1
    AS $$

BEGIN	
	RETURN QUERY SELECT users.id FROM users where users.name=name_value AND users.password=password_value;
END

$$;


ALTER FUNCTION public.check_login(name_value text, password_value text) OWNER TO postgres;

--
-- Name: clean_db(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.clean_db() RETURNS void
    LANGUAGE plpgsql
    AS $$

BEGIN	
    DELETE FROM messages;
    DELETE FROM tasks;
    DELETE FROM sources;
END

$$;


ALTER FUNCTION public.clean_db() OWNER TO postgres;

--
-- Name: get_setting(text); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_setting(name_value text) RETURNS text
    LANGUAGE plpgsql
    AS $$

DECLARE
	val text;

BEGIN	
	SELECT value FROM settings INTO val WHERE name=name_value;
	RETURN val;
END

$$;


ALTER FUNCTION public.get_setting(name_value text) OWNER TO postgres;

--
-- Name: get_source(bigint); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_source(id_value bigint) RETURNS TABLE(destination text, user_id bigint, type_id bigint, info text)
    LANGUAGE plpgsql
    AS $$

BEGIN	
	RETURN QUERY SELECT sources.destination, sources.user_id, sources.type_id, sources.info
		FROM sources WHERE sources.id=id_value;
END

$$;


ALTER FUNCTION public.get_source(id_value bigint) OWNER TO postgres;

--
-- Name: get_source_result(bigint); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_source_result(source_id_value bigint) RETURNS TABLE(result text, user_id bigint)
    LANGUAGE plpgsql
    AS $$

BEGIN	
	RETURN QUERY SELECT tasks.result, tasks.user_id FROM tasks WHERE tasks.source_id = source_id_value;
END

$$;


ALTER FUNCTION public.get_source_result(source_id_value bigint) OWNER TO postgres;

--
-- Name: get_sources_queue_size(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_sources_queue_size() RETURNS integer
    LANGUAGE plpgsql
    AS $$

DECLARE
	val integer;

BEGIN	
	SELECT COUNT(*) FROM sources INTO val WHERE status=0 AND NOT sources.id IN (SELECT tasks.source_id FROM tasks);
	RETURN val;
END

$$;


ALTER FUNCTION public.get_sources_queue_size() OWNER TO postgres;

--
-- Name: get_task(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_task() RETURNS TABLE(destination text, source_id bigint, task_id bigint, user_id bigint, info text)
    LANGUAGE plpgsql
    AS $$

DECLARE 
		destination_value text;
        source_id_value bigint;
		task_id_value bigint;
		user_id_value bigint;
		info_value text;

BEGIN	
	SELECT sources.id, sources.destination, sources.user_id, sources.info 
		INTO source_id_value, destination_value, user_id_value, info_value
		FROM sources WHERE status=0 AND NOT sources.id IN (SELECT tasks.source_id FROM tasks)
		ORDER BY created DESC LIMIT 1;
    IF NOT source_id_value IS NULL THEN
		INSERT INTO tasks (source_id, user_id, type_id)
            VALUES (source_id_value, user_id_value, 1) -- Photo Fraud Detection
            RETURNING id INTO task_id_value;
	END IF;                

	RETURN QUERY SELECT destination_value, source_id_value, task_id_value, user_id_value, info_value;
END

$$;


ALTER FUNCTION public.get_task() OWNER TO postgres;

--
-- Name: get_task_result(bigint); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.get_task_result(task_id_value bigint) RETURNS TABLE(result text)
    LANGUAGE plpgsql
    AS $$
BEGIN	
	RETURN QUERY SELECT tasks.result FROM tasks WHERE tasks.task_id = task_id_value;
END

$$;


ALTER FUNCTION public.get_task_result(task_id_value bigint) OWNER TO postgres;

--
-- Name: set_setting(text, text); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.set_setting(name_value text, value_value text) RETURNS bigint
    LANGUAGE plpgsql
    AS $$

DECLARE
	id_value bigint;

BEGIN	
	SELECT id FROM settings INTO id_value WHERE name=name_value;
	IF id_value IS NULL THEN
		INSERT INTO settings (name, value) VALUES (name_value, value_value) 
			RETURNING id INTO id_value;
	ELSE
		UPDATE settings SET value=value_value WHERE id=id_value;
	END IF;
	RETURN id_value;
END

$$;


ALTER FUNCTION public.set_setting(name_value text, value_value text) OWNER TO postgres;

--
-- Name: set_task_result(bigint, text, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.set_task_result(task_id_value bigint, json_value text, status_value integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$

DECLARE 
        source_id_value bigint;

BEGIN	
	UPDATE tasks SET result = json_value WHERE id=task_id_value;
	SELECT tasks.source_id INTO source_id_value FROM tasks WHERE tasks.id=task_id_value;
    IF NOT source_id_value IS NULL THEN
		UPDATE sources SET status = status_value WHERE id=source_id_value;
		RETURN 1;
	END IF;         
	RETURN 0;
END

$$;


ALTER FUNCTION public.set_task_result(task_id_value bigint, json_value text, status_value integer) OWNER TO postgres;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: messages; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.messages (
    id bigint NOT NULL,
    created timestamp without time zone DEFAULT now() NOT NULL,
    message text NOT NULL,
    user_id bigint,
    type_id bigint NOT NULL
);


ALTER TABLE public.messages OWNER TO postgres;

--
-- Name: messages_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.messages_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.messages_id_seq OWNER TO postgres;

--
-- Name: messages_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.messages_id_seq OWNED BY public.messages.id;


--
-- Name: seq_settings_id; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.seq_settings_id
    START WITH 1000
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.seq_settings_id OWNER TO postgres;

--
-- Name: settings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.settings (
    id bigint DEFAULT nextval('public.seq_settings_id'::regclass) NOT NULL,
    name text NOT NULL,
    value text,
    user_id bigint,
    task_id bigint,
    source_id bigint
);


ALTER TABLE public.settings OWNER TO postgres;

--
-- Name: sources; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sources (
    id bigint NOT NULL,
    destination text,
    info text,
    created timestamp without time zone DEFAULT now() NOT NULL,
    user_id bigint,
    type_id bigint NOT NULL,
    status integer DEFAULT 0 NOT NULL
);


ALTER TABLE public.sources OWNER TO postgres;

--
-- Name: sources_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sources_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.sources_id_seq OWNER TO postgres;

--
-- Name: sources_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sources_id_seq OWNED BY public.sources.id;


--
-- Name: tasks; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tasks (
    type_id bigint NOT NULL,
    source_id bigint,
    user_id bigint,
    created timestamp without time zone DEFAULT now() NOT NULL,
    result text,
    id bigint NOT NULL
);


ALTER TABLE public.tasks OWNER TO postgres;

--
-- Name: tasks_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tasks_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tasks_id_seq OWNER TO postgres;

--
-- Name: tasks_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tasks_id_seq OWNED BY public.tasks.id;


--
-- Name: types; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.types (
    name text NOT NULL,
    kind bigint NOT NULL,
    id bigint NOT NULL
);


ALTER TABLE public.types OWNER TO postgres;

--
-- Name: types_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.types_id_seq OWNER TO postgres;

--
-- Name: types_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.types_id_seq OWNED BY public.types.id;


--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    id bigint NOT NULL,
    name text NOT NULL,
    password text NOT NULL,
    type_id bigint NOT NULL
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.users_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.users_id_seq OWNER TO postgres;

--
-- Name: users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.users_id_seq OWNED BY public.users.id;


--
-- Name: messages id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.messages ALTER COLUMN id SET DEFAULT nextval('public.messages_id_seq'::regclass);


--
-- Name: sources id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sources ALTER COLUMN id SET DEFAULT nextval('public.sources_id_seq'::regclass);


--
-- Name: tasks id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tasks ALTER COLUMN id SET DEFAULT nextval('public.tasks_id_seq'::regclass);


--
-- Name: types id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.types ALTER COLUMN id SET DEFAULT nextval('public.types_id_seq'::regclass);


--
-- Name: users id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users ALTER COLUMN id SET DEFAULT nextval('public.users_id_seq'::regclass);


--
-- Name: types id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.types
    ADD CONSTRAINT id PRIMARY KEY (id);


--
-- Name: messages messages_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.messages
    ADD CONSTRAINT messages_pkey PRIMARY KEY (id);


--
-- Name: settings pkey_settings; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.settings
    ADD CONSTRAINT pkey_settings PRIMARY KEY (id);


--
-- Name: sources sources_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sources
    ADD CONSTRAINT sources_pkey PRIMARY KEY (id);


--
-- Name: tasks tasks_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tasks
    ADD CONSTRAINT tasks_pkey PRIMARY KEY (id);


--
-- Name: users users_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT users_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

